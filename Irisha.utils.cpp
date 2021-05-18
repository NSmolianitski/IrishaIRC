//
// Created by Parfait Kentaurus on 5/14/21.
//

#include "Irisha.hpp"
#include "utils.hpp"
#include "Channel.hpp"
#include <sstream>

/**
 * @description	Receives a message from socket
 * @param		socket: sender socket
 * @return		message received from socket
 */
std::string Irisha::get_msg(int sock)
{
	int read_bytes = recv(sock, &buff_, 512, 0);
	if (read_bytes < 0) throw std::runtime_error("Recv error in get_msg()");

	if (read_bytes == 0)
		handle_disconnection(sock);
	buff_[read_bytes] = '\0';
	return (buff_);
}

/**
 * @description	Finds connection by socket
 * @param		sock: socket
 * @return		Returns connection pointer or nullptr
 */
AConnection* Irisha::find_connection(const int sock) const
{
	for (con_const_it it = connections_.begin(); it != connections_.end(); ++it)
	{
		if (it->second->socket() == sock)
			return it->second;
	}
	return nullptr;
}

/**
 * @description	Finds server by socket
 * @param		sock: socket
 * @return		server pointer or nullptr
 */
Server* Irisha::find_server(const int sock) const
{
	Server*			server;
	con_const_it	it = connections_.begin();
	for (; it != connections_.end(); ++it)
	{
		if (it->second->type() == T_SERVER)
		{
			server = static_cast<Server*>(it->second);
			if (server->socket() == sock)
				return server;
		}
	}
	return nullptr;
}

/**
 * @description	Prints server information
 */
void Irisha::print_info() const
{
	std::cout << BOLD UND "Current server configuration" << CLR "\n";
	std::cout << "domain: " << ITALIC PURPLE + domain_ << CLR "\n";
	std::cout << "password: " << ITALIC PURPLE + password_ << CLR << std::endl;
}

/**
 * @description	Sends a message to socket
 * @param		sock: receiver socket
 * @param		msg: message
 */
void Irisha::send_msg(int sock, const std::string& prefix, const std::string& msg) const
{
	std::string message;
	if (!prefix.empty())
		message = ":" + prefix + " " + msg;
	else
		message = msg;

	std::cout << message + " " E_SPEECH PURPLE ITALIC " to " + connection_name(sock) << CLR << std::endl;
	message.append("\r\n");

	ssize_t n = send(sock, message.c_str(), message.length(), 0);
	if (n == -1) throw std::runtime_error("Send error");
}

/**
 * @description	Sends a message to all servers
 * @param		prefix: sender
 * @param		msg: message
 */
void Irisha::send_servers(const std::string& prefix, const std::string& msg) const
{
	con_const_it	it = connections_.begin();
	for (; it != connections_.end(); ++it)
	{
		if (it->second->type() == T_SERVER)
			send_msg(it->second->socket(), prefix, msg);
	}
}

/**
 * @description	Sends a message to all servers except one with socket
 * @param		prefix: sender
 * @param		msg: message
 * @param		sock: exception socket
 */
void Irisha::send_servers(const std::string& prefix, const std::string& msg, const int sock) const
{
	con_const_it	it = connections_.begin();
	for (; it != connections_.end(); ++it)
	{
		if (it->second->type() == T_SERVER)
			if (it->second->socket() != sock)
				send_msg(it->second->socket(), prefix, msg);
	}
}

/**
 * @description	Sends a message to all connections
 * @param		prefix: sender
 * @param		msg: message
 */
void Irisha::send_everyone(const std::string& prefix, const std::string& msg) const
{
	con_const_it	it = connections_.begin();
	for (; it != connections_.end(); ++it)
		send_msg(it->second->socket(), prefix, msg);
}

/**
 * @description	Calls IRC command if it exists
 * @param		sock
 */
void Irisha::handle_command(const int sock)
{
	std::map<std::string, func>::const_iterator it = commands_.find(cmd_.command_);
	if (it != commands_.end())	// Execute command
		((*this).*it->second)(sock);
	else
		send_msg(sock, domain_, ":No such command, my friend"); //! TODO: change to error reply (421, "<command> :Unknown command")
}
/// Send msg channel all users and operators
void Irisha::send_channel(Channel *channel, std::string msg, std::string prefix)
{
    std::vector<User*>::const_iterator itr = channel->getUsers().begin();
    std::vector<User*>::const_iterator ite = channel->getUsers().end();

    while (itr != ite)
    {
        send_msg((*itr)->socket(), prefix, msg);
        itr++;
    }
//    itr = channel->getOperators().begin();
//    ite = channel->getOperators().end();
//    while (itr != ite)
//    {
//        send_msg((*itr)->socket(), prefix, msg);
//        itr++;
//    }
    send_msg_to_servers(prefix, msg);
}

/**
 * @description	Gets connection name by socket
 * @param		sock
 * @return		connection name
 */
std::string Irisha::connection_name(const int sock) const
{
	User*	user = find_user(sock);

	std::string name;
	if (user == nullptr)
	{
		Server*	server = find_server(sock);
		if (server == nullptr)
			return "unknown";
		name = server->name();
	}
	else
		name = user->nick();
	return name;
}

/// ‼️ ⚠️ DEVELOPMENT UTILS (REMOVE OR COMMENT WHEN PROJECT IS READY) ⚠️ ‼️ //! TODO: DEV -> REMOVE /////////////////////
#define GUEST52 "Guest52" //! TODO: REMOVE

/**
 * @description	Thread loop for sending custom messages for all connections
 * @param		server: current server
 */
void sending_loop(const Irisha* server)
{
	std::string serv_prefix = ":" + server->domain_ + " ";
	std::string input;
	while (true)
	{
		getline(std::cin, input);
		if (input == "/exit" || input == "/EXIT")
			exit(0);
		if (input[0] == '!' && input[1] == ' ') // Change ! to domain name
			input = input.replace(0, 2, serv_prefix);
		else if (input[0] == 'W' && input[1] == ' ')
		{
			input = input.replace(0, 2, serv_prefix + "001 ");
			input.append(" ⭐ Welcome to Irisha server! ⭐");
		}
		else if (input == "W")
			input = serv_prefix + "001 " + GUEST52 + " ⭐ Welcome to Irisha server! ⭐";
		else if (input[0] == 'U' && input[1] == ' ')
			server->user_info(input.substr(2));
		else if (input[0] == 'U')
			server->print_user_list();
		else
		{
			for (int i = 3; i < server->max_fd_ + 1; ++i)
			{
				if (FD_ISSET(i, &server->all_fds_) && i != server->listener_)
					server->send_msg(i, NO_PREFIX, input);
			}
		}
	}
}

/**
 * @description	Prints command structure (has three modes: PM_LINE, PM_LIST and PM_ALL(both))
 */
void Irisha::print_cmd(ePrintMode mode, const int sock) const
{
	if (mode == PM_ALL)
	{
		print_cmd(PM_LINE, sock);
		print_cmd(PM_LIST, sock);
		return;
	}
	std::stringstream	sender;
	sender << connection_name(sock);
	std::cout << "[" BLUE << sender.str() << CLR "] ";
	if (!cmd_.prefix_.empty())
	{
		if (mode == PM_LIST)
			std::cout << BWHITE "PREFIX: " GREEN ITALIC + cmd_.prefix_ << CLR "\n";
		else
			std::cout << GREEN ITALIC << cmd_.prefix_ << CLR " ";
	}

	if (mode == PM_LIST)
		std::cout << BWHITE "COMMAND: " YELLOW ITALIC + cmd_.command_ << CLR "\n";
	else
		std::cout << YELLOW ITALIC << cmd_.command_ << CLR " ";

	if (mode == PM_LIST)
		std::cout << BWHITE "ARGUMENTS: ";
	std::cout << CYAN ITALIC;

	for (int i = 0; i < cmd_.arguments_.size(); ++i)
	{
		if (cmd_.arguments_[i].empty())
			std::cout << "<empty>" << " ";
		else
			std::cout << cmd_.arguments_[i] << " ";
	}
	std::cout << CLR << std::endl;
}

#include <iomanip>

/**
 * @description	Prints user information
 * @param		nick
 */
void Irisha::user_info(const std::string& nick) const
{
	User*	user = find_user(nick);
	if (user == nullptr)
	{
		std::cout << RED BOLD "User " BWHITE + nick + RED " not found!" CLR << std::endl;
		return;
	}
	std::cout << "-----------[USER INFO]-----------" << std::endl;
	std::cout << "| Nick: "		<< std::setw(23) << user->nick() << " |" << std::endl;
	std::cout << "| Username: " << std::setw(19)	<< user->username() << " |" << std::endl;
	std::cout << "| Realname: " << std::setw(19)	<< user->realname() << " |" << std::endl;
	std::cout << "| Password: " << std::setw(19)	<< user->password() << " |" << std::endl;
	std::cout << "| Mode: "		<< std::setw(23)	<< user->mode() << " |" << std::endl;
	std::cout << "| Server: "	<< std::setw(21)	<< user->server() << " |" << std::endl;
	std::cout << "| Hopcount: "	<< std::setw(19)	<< user->hopcount() << " |" << std::endl;
	std::cout << "---------------------------------" << std::endl;
}

/**
 * @description	Prints user list
 */
void Irisha::print_user_list() const
{
	User*	user;
	con_const_it it = connections_.begin();
	std::cout << "[" PURPLE "CONNECTED USERS" CLR "] " BWHITE ITALIC;
	for (; it != connections_.end(); ++it)
	{
		if (it->second->type() == T_CLIENT)
		{
			user = static_cast<User*>(it->second);
			std::cout << user->nick() << " ";
		}
	}
	std::cout << CLR << std::endl;
}

/** (unused)
 * @description	Sends a message from input
 * @param		sock: receiver socket
 */
void Irisha::send_input_msg(int sock) const
{
	std::string input;
	getline(std::cin, input);
	if (input == "/exit" || input == "/EXIT")
	{
		std::cout << ITALIC PURPLE "Server shutdown." CLR << std::endl;
		exit(0);
	}

	std::string message = ":" + domain_ + " " + input;
	message.append("\r\n");

	ssize_t n = send(sock, message.c_str(), message.length(), 0);
	if (n == -1) throw std::runtime_error("Send error");
}
/// ‼️ ⚠️ END OF DEVELOPMENT UTILS ⚠️ ‼️ //! TODO: DEV -> REMOVE //////////////////////////////////////////
