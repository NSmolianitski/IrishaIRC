
#include "Irisha.hpp"
#include "User.hpp"
#include "Server.hpp"
#include "utils.hpp"

#include <sstream>

/**
 * @description	Inserts all supported IRC commands to map
 */
void	Irisha::prepare_commands()
{
	commands_.insert(std::pair<std::string, func>("PASS", &Irisha::PASS));
	commands_.insert(std::pair<std::string, func>("NICK", &Irisha::NICK));
	commands_.insert(std::pair<std::string, func>("USER", &Irisha::USER));
	commands_.insert(std::pair<std::string, func>("SERVER", &Irisha::SERVER));
	commands_.insert(std::pair<std::string, func>("PING", &Irisha::PING));
	commands_.insert(std::pair<std::string, func>("PONG", &Irisha::PONG));
	commands_.insert(std::pair<std::string, func>("QUIT", &Irisha::QUIT));
	commands_.insert(std::pair<std::string, func>("TIME", &Irisha::TIME));
	commands_.insert(std::pair<std::string, func>("USERS", &Irisha::USERS));
	commands_.insert(std::pair<std::string, func>("KILL", &Irisha::KILL));
	commands_.insert(std::pair<std::string, func>("ADMIN", &Irisha::ADMIN));
}

/**
 * @description	Changes user nick if it's not busy
 * @param		connection: User or Server
 * @param		sock: socket
 * @param		new_nick: desired nickname
 * @return		R_SUCCESS or R_FAILURE
 */
eResult	Irisha::NICK_user(User* const connection, const int sock, const std::string& new_nick)
{
	User*		user;
	std::string	old_nick;

	old_nick = connection->nick();
	if (old_nick == new_nick)
		return R_SUCCESS;
	user = find_user(new_nick);
	if (user)
	{
		err_nicknameinuse(sock, new_nick);
		return R_FAILURE;
	}
	send_msg(sock, old_nick, "NICK " + new_nick); // Reply for user about nick changing success
	connection->set_nick(new_nick);
	// TODO: send message to next server

	sys_msg(E_GEAR, "User", old_nick, "changed nick to", new_nick);
	return R_SUCCESS;
}

/**
 * @description	Handles request from other server (changes nick or adds new user)
 * @param		new_nick: new nickname
 * @return		R_SUCCESS
 */
eResult	Irisha::NICK_server(const std::string& new_nick, int source_sock)
{
	User*		user;
	std::string	old_nick;

	user = find_user(cmd_.prefix_);
	if (user == nullptr)		// Add new external user
	{
		add_user(source_sock);
		//sending NICK msg to other servers
		Server* serv = static_cast<Server*>(find_server(source_sock));
		send_servers(cmd_.prefix_, "NICK " + cmd_.arguments_[0] + " " + std::to_string(serv->hopcount() + 1) +
			" " + cmd_.arguments_[2] + " " + cmd_.arguments_[3] + " " + std::to_string(serv->token() + 1) +
			" " + cmd_.arguments_[5] + cmd_.arguments_[6], source_sock);
		return R_SUCCESS;
	}
	old_nick = user->nick();
	user->set_nick(new_nick);	// Change nick for external user
	sys_msg(E_GEAR, "User", old_nick, "changed nick to", new_nick);
	// TODO: send message to next server

	return R_SUCCESS;
}

/**
 * @description	Handles NICK command
 * @param		sock: command sender socket
 */
eResult	Irisha::NICK(const int sock) //! TODO: handle hopcount
{
	if (cmd_.arguments_.empty())	// NICK command without params
	{
		err_nonicknamegiven(sock);
		return R_FAILURE;
	}
	std::string new_nick = cmd_.arguments_[0];
	if (!is_a_valid_nick(new_nick))
	{
		err_erroneusnickname(sock, new_nick);
		return R_FAILURE;
	}

	con_it it = connections_.begin();
	for (; it != connections_.end(); ++it)
	{
		if (it->second->socket() == sock)
			break;
	}
	if (it == connections_.end())	// Add new local user
		add_user(sock, new_nick);
	else
	{
		User*	connection = dynamic_cast<User *>(it->second);
		if (connection == nullptr)	// Handle request from other server
			return NICK_server(new_nick, sock);
		return NICK_user(connection, sock, new_nick); // Change local user nickname
	}
	print_user_list(); //! TODO: remove
	return R_SUCCESS;
}

/**
 * @description	Handles USER command
 * @param		sock: command sender socket
 */
eResult Irisha::USER(const int sock)
{
	if (cmd_.arguments_.size() < 4)
	{
		err_needmoreparams(sock, "USER");
		return R_FAILURE;
	}
	User*	user = find_user(sock);
	if (user == nullptr)	// Safeguard for invalid user
	{
		std::cout << RED BOLD "ALARM! WE DON'T HAVE USER WITH SOCKET" BWHITE " №" << sock << RED " IN OUR DATABASE!" CLR << std::endl;
		return R_FAILURE;
	}
	if (!user->username().empty())
	{
		err_alreadyregistered(sock);
		return R_FAILURE;
	}
	user->set_username(cmd_.arguments_[0]);

	if (cmd_.arguments_[1].length() > 1) //! TODO: don't forget to handle modes
		user->set_mode(0);
	else
		user->set_mode(str_to_int(cmd_.arguments_[1]));
	rpl_welcome(sock);

	sys_msg(E_MAN, "New local user", user->nick(), "registered!");
	// NICK <nickname> <hopcount> <username> <host> <servertoken> <umode> <realname>
	send_servers(domain_, "NICK " + user->nick() + " 1 " + user->username() + " " + user->server() + " 1 + " + user->username()); //! TODO: add user modes, fix realname
	return R_SUCCESS;
}

/**
 * @description	Handling PASS message
 * @param		sock: command sender socket
 * @return		R_SUCCESS if password correct, else R_FAILURE
 */
eResult Irisha::PASS(const int sock)
{
	if (cmd_.arguments_.empty())
		return R_FAILURE;
	else if (password_ == cmd_.arguments_[0] || !cmd_.prefix_.empty())
		return R_SUCCESS;
	else
		return R_FAILURE;
}

/**
 * @description	Handling SERVER message
 * @param		sock: command sender socket
 * @return		R_FAILURE if registration successfully, else R_FAILURE
 */
eResult Irisha::SERVER(const int sock) ///TODO: test server tokens!
{
	//validation+
	if (cmd_.arguments_.empty())
		return R_FAILURE;
	if (cmd_.arguments_[cmd_.arguments_.size() - 1].find(':') == std::string::npos)
		return R_FAILURE;
	//validation-

	int hopcount;
	int token;
	//only server name sent
	if (cmd_.arguments_.size() == 2)
	{
		hopcount = 1;
		token = next_token();
	}
	//token and server name sent
	else if (cmd_.arguments_.size() == 3 &&
		(cmd_.arguments_[1].find_first_not_of("0123456789") == std::string::npos))
	{
		hopcount = 1;
		try
		{
			token = std::stoi(cmd_.arguments_[1]);
		}
		catch(std::exception ex) { return R_FAILURE; }
	}
	//servername, hopcount, token sent
	else if (cmd_.arguments_.size() == 4
				&& (cmd_.arguments_[1].find_first_not_of("0123456789") == std::string::npos)
				&& (cmd_.arguments_[2].find_first_not_of("0123456789") == std::string::npos))
	{
		try
		{
			hopcount = std::stoi(cmd_.arguments_[1]);
			token = std::stoi(cmd_.arguments_[2]);
		}
		catch(std::exception ex) { return R_FAILURE; }
	}

	if (find_server(sock) == nullptr)	//new connection to this server
	{
		AConnection* server = new Server(cmd_.arguments_[0], sock, hopcount, token, sock);
		connections_.insert(std::pair<std::string, AConnection*>(cmd_.arguments_[0], server));
		if (sock != parent_fd_)
		{
			send_msg(sock, NO_PREFIX, createPASSmsg(password_));
			send_msg(sock, NO_PREFIX, createSERVERmsg(nullptr));

			//send information about other servers to connected serer
			//and about connected server to other servers
			std::map<std::string, AConnection*>::iterator it = connections_.begin();
			for (; it != connections_.end(); it++)
			{
				if (it->second->type() == T_SERVER && sock != it->second->socket() )
				{
					send_msg(sock, domain_, createSERVERmsg(it->second));
					if (it->second->socket() != U_EXTERNAL_CONNECTION)
						send_msg(it->second->socket(), domain_, createSERVERmsg(server));
					++token;
				}
			}
			//send information about other clients to connected server
//			it = connections_.begin();
//			for (; it != connections_.end(); it++)
//			{
//				if (it->second->type() == T_CLIENT)
//				{
//					User* usr = static_cast<User*>(it->second);
//					if (usr->socket() != U_EXTERNAL_CONNECTION)
//					{
//						send_msg(sock, domain_, "NICK " + usr->nick() + " 1 " + usr->username() + " " +
//							usr->);
//					}
//				}
//			}
		}
		PING(sock);
	}
	else	//handle message from known server about new server
	{
		AConnection* server = new Server(cmd_.arguments_[0], U_EXTERNAL_CONNECTION, hopcount, token, sock);
		connections_.insert(std::pair<std::string, AConnection*>(cmd_.arguments_[0], server));

		//send to connected servers about new server, except current server and server-sender this message
		std::map<std::string, AConnection*>::iterator it = connections_.begin();
		for (; it != connections_.end(); it++)
		{
			if (it->second->type() == T_SERVER && sock != it->second->socket() &&
				 it->second->socket() != U_EXTERNAL_CONNECTION)
			{
				send_msg(it->second->socket(), find_server(sock)->name(), createSERVERmsg(server));
			}
		}
	}
	sys_msg(E_LAPTOP, "Server", cmd_.arguments_[0], "registered!");
	return R_SUCCESS;
}

/**
 * @description	Returns PASS message string
 * @param		password - parent server password for connection
 * @return		PASS command string in this format: PASS <password> <version> <flags>
 */
std::string Irisha::createPASSmsg(std::string password) const
{
	std::string msg = "PASS ";
	msg.append(password);
	msg.append(" 0210 IRC| ");
	return msg;
}

/**
 * @description	Returns SERVER message string
 * @return		SERVER command string in this format: <servername> <info>
 */
std::string Irisha::createSERVERmsg(AConnection* server) const	///TODO: choose servername smarter
{
	std::string msg;
	if (server == nullptr)
	{
		msg = "SERVER " + domain_ + " 1 :Irisha server";
	}
	else
	{
		Server* serv = static_cast<Server*>(server);
		msg = "SERVER " + serv->name() +
				" " + std::to_string(serv->hopcount() + 1) +
				" " + std::to_string(serv->token() + 1) + " :Irisha server";
	}
	return msg;
}

eResult Irisha::PONG(const int sock)
{
	if (cmd_.arguments_.empty())
	{
		err_noorigin(sock);
		return R_FAILURE;
	}
	else if (cmd_.arguments_.size() == 1 && cmd_.command_ == "PING")
		send_msg(sock, domain_, "PONG " + cmd_.prefix_);
	//else send to receiver
	return R_SUCCESS;
}

eResult Irisha::PING(const int sock)
{
	if (cmd_.arguments_.empty())
	{
		err_noorigin(sock);
		return R_FAILURE;
	}
	else if (cmd_.arguments_.size() == 1 && cmd_.command_ == "PING") // PINGing this server
		PONG(sock);
	else if (cmd_.command_ == "SERVER")
		send_msg(sock, domain_,"PING :" + domain_);
	return R_SUCCESS;
}

eResult Irisha::QUIT(const int sock)
{
	User*	user;
	bool	local = true;
	if (cmd_.prefix_.empty())
		user = find_user(sock);
	else
	{
		user = find_user(cmd_.prefix_);
		local = false;
	}

	std::string	msg;
	if (user == nullptr)
	{
		sys_msg(E_CROSS, "Can't disconnect user", cmd_.prefix_, "(don't worry, not our problem)");
		return R_FAILURE;
	}
	msg = sys_msg(E_SCULL, "User", user->nick(), "disconnected!");
	if (!cmd_.arguments_.empty())
		msg = cmd_.arguments_[0];

	if (local)
	{
		send_servers(user->nick(), msg);
		FD_CLR(sock, &all_fds_);
		close(sock);
	}
	else
		send_servers(user->nick(), msg, sock);
	remove_user(user->nick());
	return R_SUCCESS;
}

eResult Irisha::TIME(const int sock)
{
	time_t	current_time = time(nullptr);
	std::string local_time = ctime(&current_time);
	local_time = local_time.substr(0, local_time.length() - 1);
	if (!cmd_.arguments_.empty())
	{
		if (find_server(cmd_.arguments_[0]) == nullptr)
		{
			err_nosuchserver(sock, cmd_.arguments_[0]);
			return R_FAILURE;
		}
		rpl_time(sock, cmd_.arguments_[0], local_time);
		return R_SUCCESS;
	}
	rpl_time(sock, domain_, local_time);
	return R_SUCCESS;
}

eResult Irisha::USERS(const int sock)
{
	err_usersdisabled(sock);
	return R_SUCCESS;
}

eResult Irisha::KILL(const int sock)
{
	if (cmd_.arguments_.size() < 2)
	{
		err_needmoreparams(sock, "KILL");
		return R_FAILURE;
	}
	//! TODO: make KILL command
	return R_SUCCESS;
}

void	Irisha::admin_info(const int sock, const std::string& receiver)
{
	rpl_adminme(sock, receiver, domain_);
	rpl_adminloc1(sock, receiver, admin_location_);
	rpl_adminloc2(sock, receiver, admin_info_);
	rpl_adminmail(sock, receiver, admin_mail_);
}

eResult Irisha::ADMIN(const int sock)
{
	if (cmd_.arguments_.empty())
	{
		admin_info(sock, connection_name(sock));
		return R_SUCCESS;
	}
	Server* server;
	for (int i = 0; i < cmd_.arguments_.size(); ++i)
	{
		server = find_server(cmd_.arguments_[i]);
		if (cmd_.arguments_[i] == domain_)
			admin_info(sock, cmd_.prefix_);
		else if (server == nullptr)
			err_nosuchserver(sock, cmd_.arguments_[i]);
		else
			send_servers(connection_name(sock), "ADMIN " + cmd_.arguments_[i], sock);
			//! TODO: send ADMIN <nick> cmd_.arguments_[i] to other servers
	}
	return R_SUCCESS;
}

/*
 * OPER
 * SERVICE
 * SQUIT
 * NAMES
 * LIST
 * INVITE
 * KICK
 * NOTICE
 * MOTD
 * LUSERS
 * VERSION
 * STATS
 * LINKS
 * CONNECT
 * TRACE
 * ADMIN
 * INFO
 * SERVLIST
 * SQUERY
 * WHO
 * WHOIS
 * WHOWAS
 * KILL
 * ERROR
 */