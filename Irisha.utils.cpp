
#include "Irisha.hpp"
#include "Channel.hpp"
#include "utils.hpp"

#include <sstream>
#include <iomanip>

/**
 * Determines user which sent message. If command prefix is empty determines by socket, else by prefix
 * @param sock
 * @return user pointer if it found, else 0
 */
User* Irisha::determine_user(int sock)
{
	User* user;
	if (cmd_.prefix_.empty()) //got command from user
		user = find_user(sock);
	else
		user = find_user(cmd_.prefix_);
	return user;
}

/**
 * send information about all known servers to socket
 * @param sock
 */
void Irisha::send_servers_info(int sock)
{
	std::map<std::string, AConnection*>::iterator it = connections_.begin();
	for (; it != connections_.end(); it++)
	{
		if (it->second->type() == T_SERVER && sock != it->second->socket() )
		{
			send_msg(sock, domain_, createSERVERmsg(it->second));
		}
	}
}

/**
 * send information about all known clients to socket
 * @param sock
 */
void Irisha::send_clients_info(int sock)
{
	std::map<std::string, AConnection*>::iterator it = connections_.begin();
	for (; it != connections_.end(); it++)
	{
		if (it->second->type() == T_CLIENT)
		{
			User* usr = static_cast<User*>(it->second);
			send_msg(sock, domain_, createNICKmsg(usr));
		}
	}
}

eResult Irisha::resend_msg(int sock)
{
	AConnection* target = find_connection(cmd_.arguments_[0]);
	if (target == 0)
		return R_FAILURE;
	send_msg(choose_sock(target), cmd_.line_);
	return R_SUCCESS;
}

/**
 * returns socket to send message for connection
 * @param where will message sent
 * @return socket
 */
int Irisha::choose_sock(AConnection *connection)
{
	if (connection->socket() == U_EXTERNAL_CONNECTION)
		return connection->source_socket();
	else
		return connection->socket();
}

std::string*	Irisha::choose_buff(int sock, std::list<Irisha::RegForm*>& reg_expect)
{
	std::string*	buff;
	AConnection*	sender = find_connection(sock);
	if (sender == nullptr)
	{
		RegForm*	form = find_regform(sock, reg_expect);
		if (form != nullptr)
		{
			buff = &form->buff_;
			form->last_msg_time_ = time(nullptr);
		}
		else
			return &buff_;
	}
	else
	{
		buff = &sender->buff();
		sender->update_time();
	}
	return buff;
}

/**
 * find biggest token and return it + 1
 * @return token for new server connection
 */
int 			Irisha::next_token()
{
	int max = 0;

	std::map<std::string, AConnection*>::iterator it = connections_.begin();
	for (; it != connections_.end(); it++)
	{
		if (it->second->type() == T_SERVER)
		{
			Server *server = dynamic_cast<Server*>(it->second);
			if (server != 0)
				max = (server->token() > max) ? server->token() : max;
		}
	}
	return (max + 1);
}

/**
 * @description	Receives a message from socket
 * @param		socket: sender socket
 * @return		message received from socket
 */
std::string* Irisha::get_msg(int sock, std::list<Irisha::RegForm*>& reg_expect)
{
	char			tmp_buff[513];

    cmd_.type_ = connection_type(sock);
	int read_bytes = recv(sock, &tmp_buff, 512, 0);
	if (read_bytes < 0)
		throw std::runtime_error("Recv error in get_msg()");

	if (read_bytes == 0)
		close_connection(sock, "connection lost", &reg_expect);
	else if (read_bytes > 510)
	{
		send_msg(sock, domain_, "Error! Request is too long");
		read_bytes = 0;
	}
	tmp_buff[read_bytes] = '\0';

	std::string*	buff = choose_buff(sock, reg_expect);

	*buff += tmp_buff;
	return buff;
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
 * @description	Finds connection by name
 * @param		sock: socket
 * @return		Returns connection pointer or nullptr
 */
AConnection* Irisha::find_connection(const std::string& name) const
{
	for (con_const_it it = connections_.begin(); it != connections_.end(); ++it)
	{
		if (it->first == name)
			return it->second;
	}
	return nullptr;
}

/**
 * @description Finds server by name
 * @param		name
 * @return		user pointer or nullptr
 */
Server* Irisha::find_server(const std::string& name) const
{
	Server*			server;
	con_const_it	it = connections_.begin();
	for (; it != connections_.end(); ++it)
	{
		if (it->second->type() == T_SERVER)
		{
			server = static_cast<Server*>(it->second);
			if (server->name() == name)
				return server;
		}
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

	std::cout << time_stamp() + message + " " E_SPEECH PURPLE ITALIC " to "
								+ connection_name(sock) << CLR << std::endl;
	message.append("\r\n");

	ssize_t n = send(sock, message.c_str(), message.length(), 0);
	if (n == -1) throw std::runtime_error("Send error");
}

/**
 * @description	Sends a message without prefix to socket
 * @param		sock: receiver socket
 * @param		msg: message
 */
void Irisha::send_msg(int sock, const std::string& msg) const
{
	std::string message = msg;

	std::cout << time_stamp() + message + " " E_SPEECH PURPLE ITALIC " to "
				 + connection_name(sock) << CLR << std::endl;
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
		if (it->second->type() == T_SERVER && it->second->socket() != U_EXTERNAL_CONNECTION)
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
			if (it->second->socket() != sock && it->second->socket() != U_EXTERNAL_CONNECTION)
				send_msg(it->second->socket(), prefix, msg);
	}
}

/**
 * @description	Sends a message without prefix to all servers except one with socket
 * @param		msg: message
 * @param		sock: exception socket
 */
void Irisha::send_servers(const std::string& msg, const int sock) const
{
	con_const_it	it = connections_.begin();
	for (; it != connections_.end(); ++it)
	{
		if (it->second->type() == T_SERVER)
			if (it->second->socket() != sock && it->second->socket() != U_EXTERNAL_CONNECTION)
				send_msg(it->second->socket(), msg);
	}
}

/**
 * @description	Sends a message to all local connections
 * @param		prefix: sender
 * @param		msg: message
 */
void Irisha::send_everyone(const std::string& prefix, const std::string& msg) const
{
	con_const_it	it = connections_.begin();
	for (; it != connections_.end(); ++it)
	{
		if (it->second->socket() != U_EXTERNAL_CONNECTION)
			send_msg(it->second->socket(), prefix, msg);
	}
}

/**
 * @description	Calls IRC command if it exists
 * @param		sock
 */
void Irisha::handle_command(const int sock)
{
	if (!is_valid_prefix(sock))
		return;
	std::map<std::string, func>::const_iterator it = commands_.find(cmd_.command_);
	if (it != commands_.end())	// Execute command
		((*this).*it->second)(sock);
	else
		err_unknowncommand(sock, cmd_.command_);
}

void Irisha::send_local_channel(Channel *channel, std::string msg, std::string prefix, int sock)
{
    std::vector<User*>::const_iterator itr = channel->getUsers().begin();
    std::vector<User*>::const_iterator ite = channel->getUsers().end();

    while (itr != ite)
    {
        if ((*itr)->socket() != U_EXTERNAL_CONNECTION && (*itr)->socket() != sock) {
            send_msg((*itr)->socket(), prefix, msg);
        }
        itr++;
    }
}
/// Send msg channel all users and operators
void Irisha::send_channel(Channel *channel, std::string msg, std::string prefix)
{
    std::vector<User*>::const_iterator itr = channel->getUsers().begin();
    std::vector<User*>::const_iterator ite = channel->getUsers().end();

    while (itr != ite)
    {
        if ((*itr)->socket() != U_EXTERNAL_CONNECTION) {
            send_msg((*itr)->socket(), prefix, msg);
        }
        itr++;
    }
    send_servers(prefix, msg);
}

void Irisha::send_channel(Channel *channel, std::string msg, std::string prefix, int sock)
{
    std::vector<User*>::const_iterator itr = channel->getUsers().begin();
    std::vector<User*>::const_iterator ite = channel->getUsers().end();

    while (itr != ite)
    {
        if ((*itr)->socket() != U_EXTERNAL_CONNECTION && (*itr)->socket() != sock)
            send_msg((*itr)->socket(), prefix, msg);
        itr++;
    }
    send_servers(prefix, msg, sock);
}

eType Irisha::connection_type(int sock)
{
    AConnection* pointer = find_connection(sock);

    if (pointer == nullptr)
        return T_NONE;
    if (pointer->type() == T_SERVER)
        return T_SERVER;
    if (pointer->type() == T_CLIENT && sock == U_EXTERNAL_CONNECTION)
        return T_CLIENT;
    if (pointer->type() == T_CLIENT)
        return T_LOCAL_CLIENT;
    return T_NONE;
}

/**
 * @description Fills user and sender pointers if they exists, sends err_nosuchnick if not
 * @param       sender_sock
 * @param       sender
 * @param       sender_name
 * @param       user
 * @param       user_nick
 * @return      R_SUCCESS or R_FAILURE
 */
eResult Irisha::check_user_sender(int sender_sock, User*& sender, const std::string& sender_name
                                  , User*& user, const std::string& user_nick)
{
    user = find_user(user_nick);
    if (user == nullptr)
    {
        err_nosuchnick(sender_sock, user_nick);
        return R_FAILURE;
    }
    sender = find_user(sender_sock);
    if (sender == nullptr)
    {
        sender = find_user(sender_name);
        if (sender == nullptr)
        {
            err_nosuchnick(sender_sock, sender_name);
            return R_FAILURE;
        }
    }
    return R_SUCCESS;
}

/**
 * @description	Tries to find user by socket, then by nick
 * @param		sock: socket to send err_nosuchnick if user doesn't exist
 * @param		user: user pointer
 * @return 		R_SUCCESS if user exists, R_FAILURE if not
 */
eResult Irisha::check_user(int sock, User*& user, const std::string& nick)
{
    user = find_user(sock);
    if (user == nullptr)
    {
        user = find_user(nick);
        if (user == nullptr)
        {
            err_nosuchnick(sock, nick);
            return R_FAILURE;
        }
    }
    return R_SUCCESS;
}

/**
 * @description	Checks server for nullptr
 * @param		sock: socket to send err_nosuchserver if server doesn't exist
 * @param		server: server pointer to check
 * @return 		R_SUCCESS if server exists, R_FAILURE if not
 */
eResult Irisha::check_server(int sock, Server*& server, const std::string& name)
{
    if (server == nullptr)
    {
		err_nosuchserver(sock, name);
		return R_FAILURE;
    }
    return R_SUCCESS;
}

/**
 * @description Checks if arguments number are at least equal to min_args_number
 * @param       sock
 * @param       command
 * @param       min_args_number: minimum arguments number
 * @return      True if args number equal or more than min_args_number
 */
bool Irisha::is_enough_args(int sock, const std::string& command, int min_args_number)
{
    if (cmd_.arguments_.size() >= min_args_number)
        return true;
    else
    {
        err_needmoreparams(sock, command);
        return false;
    }
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

/**
 * @description	Gets connection name by AConnection pointer
 * @param		connection: AConnection pointer
 * @return		connection name
 */
std::string Irisha::connection_name(AConnection* connection) const
{
	User*	user = dynamic_cast<User*>(connection);

	std::string name;
	if (user == nullptr)
	{
		Server*	server = dynamic_cast<Server*>(connection);
		if (server == nullptr)
			return "unknown";
		name = server->name();
	}
	else
		name = user->nick();
	return name;
}

void Irisha::ping_connections(time_t& last_ping)
{
	if (connections_.empty())
		return;
	std::cout << PURPLE ITALIC << "Ping connections!" CLR << std::endl;

	AConnection*	connection;
	for (con_it it = connections_.begin(); it != connections_.end();)
	{
		connection = it->second;
		int time = static_cast<int>(connection->last_msg_time());
		if (connection->socket() != U_EXTERNAL_CONNECTION && time >= ping_timeout_)
		{
			if (connection->last_msg_time() >= conn_timeout_)
			{
				++it;
				close_connection(connection->socket(), "timeout", nullptr);
				continue;
			}
			send_msg(it->second->socket(), domain_, "PING " + domain_); // Send PING message
		}
		++it;
	}
	last_ping = time(nullptr);
}

bool Irisha::is_valid_prefix(const int sock)
{
	User*	user = find_user(sock);
	if (user != nullptr && !cmd_.prefix_.empty() && cmd_.prefix_ != user->nick())
	{
		if (find_connection(cmd_.prefix_))
		{
			send_msg(sock, domain_, "Error! Cheater! Closing connection...");
			close_connection(sock, "using someone's prefix", nullptr);
		}
		else
			send_msg(sock, domain_, "Error! Wrong prefix!");
		return false;
	}
	return true;
}

///	System messages
/**
 * @description	Sends message to STDOUT
 * @param		emoji: emoji message prefix
 * @param		str: purple message
 * @return		sent string
 */
std::string	Irisha::sys_msg(const std::string& emoji, const std::string& str) const
{
	std::string msg = emoji + " " PURPLE ITALIC + str + CLR;
	std::cout << time_stamp() + msg << std::endl;
	return msg;
}

/**
 * @description	Sends message to STDOUT
 * @param		emoji: emoji message prefix
 * @param		str: purple message
 * @param		white_str: string which would be white
 * @return		sent string
 */
std::string	Irisha::sys_msg(const std::string& emoji, const std::string& str
		, const std::string& white_str) const
{
	std::string msg =  emoji + " " PURPLE ITALIC + str
					   + " " BWHITE + white_str + CLR;
	std::cout << time_stamp() + msg << std::endl;
	return msg;
}

/**
 * @description	Sends message to STDOUT
 * @param		emoji: emoji message prefix
 * @param		str: purple message
 * @param		white_str: string which would be white
 * @param		ending: purple ending
 * @return		sent string
 */
std::string	Irisha::sys_msg(const std::string& emoji, const std::string& str
		, const std::string& white_str, const std::string& ending) const
{
	std::string msg = emoji + " " PURPLE ITALIC + str
					  + " " BWHITE + white_str + " " + PURPLE + ending + CLR;
	std::cout << time_stamp() + msg << std::endl;
	return msg;
}

/**
 * @description	Sends message to STDOUT
 * @param		emoji: emoji message prefix
 * @param		str: purple message
 * @param		white_str: string which would be white
 * @param		str2: second purple string
 * @param		ending: white ending
 * @return		sent string
 */
std::string	Irisha::sys_msg(const std::string& emoji, const std::string& str
		, const std::string& white_str, const std::string& str2, const std::string& ending) const
{
	std::string msg =  emoji + " " PURPLE ITALIC + str
					   + " " BWHITE + white_str + " " + PURPLE + str2
					   + " " BWHITE + ending + CLR;
	std::cout << time_stamp() + msg << std::endl;
	return msg;
}

void Irisha::close_connection(const int sock, const std::string& comment, std::list<Irisha::RegForm*>* reg_expect)
{
	if (sock == U_EXTERNAL_CONNECTION)
	{
		std::cout << E_CROSS RED " ALARM! TRYING TO CLOSE EXTERNAL CONNECTION! " E_CROSS CLR << std::endl;
		return;
	}

	//check registration expecting connections
	if (reg_expect != nullptr)
	{
		std::list<RegForm*>::iterator it = expecting_registration(sock, *reg_expect);
		if (it != reg_expect->end())
		{
			reg_expect->erase(it);
			delete *it;
		}
	}

	User*		user = find_user(sock);
	if (user == nullptr)
	{
		Server*		server = find_server(sock);
		std::string	name = "unknown";
		if (server != nullptr)
		{
			name = server->name();
			remove_local_server(server);
			send_servers(name, "SQUIT " + name + " :" + comment, sock);
		}
		if (name == "unknown")
			sys_msg(E_BOOM, "Unknown connection closed!"); // Handle non-registered connection
		else
			sys_msg(E_BOOM, "Server", name, "disconnected!"); // Handle server connection
	}
	else
	{
		sys_msg(E_SCULL, "User", user->nick(), "disconnected!"); // Handle user connection
		send_servers(user->nick(), "QUIT :" + comment);
		remove_user(user->nick());
	}
	FD_CLR(sock, &all_fds_);
	close(sock);
}

void Irisha::remove_server(const std::string& name)
{
	Server* server = find_server(name);
	if (server == nullptr)
	{
		std::cout << E_CROSS RED "Can't remove server " + name + CLR << std::endl;
		return;
	}
	if (server->socket() != U_EXTERNAL_CONNECTION)
	{
		FD_CLR(server->socket(), &all_fds_);
		close(server->socket());
	}
	remove_server_users(server->name());
	connections_.erase(name);
	delete server;
}

void Irisha::remove_server(Server*& server)
{
	if (server == nullptr)
	{
		std::cout << E_CROSS RED "Can't remove server " + server->name() + CLR << std::endl;
		return;
	}
	if (server->socket() != U_EXTERNAL_CONNECTION)
	{
		FD_CLR(server->socket(), &all_fds_);
		close(server->socket());
	}
	remove_server_users(server->name());
	connections_.erase(server->name());
	delete server;
}

/**
 * @description	Removes local server with all its (users and servers)
 * @param		server: server pointer
 */
void Irisha::remove_local_server(Server*& server)
{
	int sock = choose_sock(server); // Socket of the desired server

	Server*	tmp;
	for (con_it it = connections_.begin(); it != connections_.end();)
	{
		if (it->second->type() == T_SERVER && choose_sock(it->second) == sock)
		{
			tmp = static_cast<Server*>(it->second);
			++it;
			remove_server(tmp);
			continue;
		}
		++it;
	}
}

/**
 * @description	Checks if user is IRC-operator by socket, sends err_noprivileges if not
 * @param		sock
 * @return		true if user is IRC-operator
 */
bool Irisha::is_user_operator(const int sock)
{
	User*	user = find_user(sock);
	if (user == nullptr)
		return false;
	else if (!user->is_operator())
	{
		err_noprivileges(sock);
		return false;
	}
	return true;
}

/// ‼️ ⚠️ DEVELOPMENT UTILS (REMOVE OR COMMENT WHEN PROJECT IS READY) ⚠️ ‼️ //! TODO: DEV -> REMOVE /////////////////////

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
	std::cout << time_stamp() << "[" BLUE << sender.str() << CLR "] ";
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

Irisha::RegForm* Irisha::find_regform(int sock, std::list<Irisha::RegForm*>& reg_expect)
{
	std::list<Irisha::RegForm*>::iterator it = reg_expect.begin();
	for (; it != reg_expect.end(); ++it)
	{
		if ((*it)->socket_ == sock)
			return *it;
	}
	return nullptr;
}

std::string Irisha::time_stamp() const
{
	if (time_stamp_ == U_DISABLED)
		return "";
	std::string msg = int_to_str(time(nullptr) - launch_time_);
	return "[" + msg + "] ";
}

void Irisha::send_channels(int sock)
{
    std::map<std::string ,Channel*>::iterator itr = channels_.begin();
    std::vector<User*>::const_iterator itr_u;
    std::string users;

    while (itr != channels_.end())
    {
        itr_u = itr->second->getOperators().begin();
        while (itr_u != itr->second->getOperators().end())
        {
            users.push_back('@');
            users.append((*itr_u)->nick());
            users.push_back(',');
            itr_u++;
        }
        itr_u = itr->second->getModerators().begin();
        while (itr_u != itr->second->getModerators().end())
        {
            users.push_back('+');
            users.append((*itr_u)->nick());
            users.push_back(',');
            itr_u++;
        }
        itr_u = itr->second->getUsers().begin();
        while (itr_u != itr->second->getUsers().end())
        {
            users.append((*itr_u)->nick());
            users.push_back(',');
            itr_u++;
        }
        if (!users.empty())
            users.erase(users.size() - 1);
        send_msg(sock, domain_, "NJOIN " + itr->first + " :" + users);
        send_msg(sock, domain_, "TOPIC " + itr->first + " " + itr->second->getTopic());
        send_msg(sock, domain_, "MODE " + itr->first + " +" + itr->second->getListMode());
        itr++;
    }
}
/// ‼️ ⚠️ END OF DEVELOPMENT UTILS ⚠️ ‼️ //! TODO: DEV -> REMOVE //////////////////////////////////////////