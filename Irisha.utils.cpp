//
// Created by Parfait Kentaurus on 5/14/21.
//

#include "Irisha.hpp"
#include "utils.hpp"

#include <sstream>

/**
 * @description	Receives a message from socket
 * @param		socket: sender socket
 * @return		message received from socket
 */
std::string Irisha::get_msg(int sock)
{
	int read_bytes = recv(sock, &buff_, 510, 0);
	if (read_bytes < 0) throw std::runtime_error("Recv error in get_msg()");

	if (read_bytes == 0)
		handle_disconnection(sock);
	buff_[read_bytes] = '\0';
	return (buff_);
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

	std::cout << message + " " E_SPEECH PURPLE ITALIC " to socket №" << sock << CLR << std::endl;
	message.append("\r\n");

	ssize_t n = send(sock, message.c_str(), message.length(), 0);
	if (n == -1) throw std::runtime_error("Send error");
}

/**
 * @description	Calls IRC command if it exists
 * @param		sock
 */
void Irisha::handle_command(int sock)
{
	std::map<std::string, func>::const_iterator it = commands_.find(cmd_.command_);
	if (it != commands_.end())	// Execute command
		((*this).*it->second)(sock);
	else
		send_msg(sock, domain_, ":No such command, my friend"); //! TODO: change to error reply (421, "<command> :Unknown command")
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
		{
			input = serv_prefix + "001 " + GUEST52 + " ⭐ Welcome to Irisha server! ⭐";
		}
		for (int i = 3; i < server->max_fd_ + 1; ++i)
		{
			if (FD_ISSET(i, &server->all_fds_) && i != server->listener_)
				server->send_msg(i, NO_PREFIX, input);
		}
	}
}

/**
 * @description	Prints command structure (has three modes: PM_LINE, PM_LIST and PM_ALL(both))
 */
void Irisha::print_cmd(PrintMode mode, const int sock) const
{
	if (mode == PM_ALL)
	{
		print_cmd(PM_LINE, sock);
		print_cmd(PM_LIST, sock);
		return;
	}

	std::stringstream	sender;
	User*				user = find_user(sock);
	if (user)	// If user exists
		sender << user->nick();
	else
		sender << "Connection №" << sock;
	if (mode == PM_LIST)
		std::cout << BWHITE "COMMAND: " YELLOW ITALIC + cmd_.command_ << CLR "\n";
	else
	{
		std::cout << "[" BLUE << sender.str() << CLR "] ";
		std::cout << YELLOW ITALIC << cmd_.command_ << CLR " ";
	}

	if (mode == PM_LIST)
		std::cout << BWHITE "ARGUMENTS: ";
	std::cout << CYAN ITALIC;

	for (int i = 0; i < cmd_.arguments_.size(); ++i)
		std::cout << cmd_.arguments_[i] << " ";
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
