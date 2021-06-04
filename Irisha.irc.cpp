
#include "Irisha.hpp"
#include "User.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "utils.hpp"
#include "parser.hpp"

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
	commands_.insert(std::pair<std::string, func>("JOIN", &Irisha::JOIN));
	commands_.insert(std::pair<std::string, func>("NJOIN", &Irisha::NJOIN));
	commands_.insert(std::pair<std::string, func>("MODE", &Irisha::MODE));
	commands_.insert(std::pair<std::string, func>("PART", &Irisha::PART));
	commands_.insert(std::pair<std::string, func>("QUIT", &Irisha::QUIT));
	commands_.insert(std::pair<std::string, func>("TOPIC", &Irisha::TOPIC));
	commands_.insert(std::pair<std::string, func>("PRIVMSG", &Irisha::PRIVMSG));
	commands_.insert(std::pair<std::string, func>("NOTICE", &Irisha::NOTICE));
	commands_.insert(std::pair<std::string, func>("NAMES", &Irisha::NAMES));
	commands_.insert(std::pair<std::string, func>("LIST", &Irisha::LIST));
	commands_.insert(std::pair<std::string, func>("KICK", &Irisha::KICK));
	commands_.insert(std::pair<std::string, func>("INVITE", &Irisha::INVITE));
	commands_.insert(std::pair<std::string, func>("TIME", &Irisha::TIME));
	commands_.insert(std::pair<std::string, func>("USERS", &Irisha::USERS));
	commands_.insert(std::pair<std::string, func>("KILL", &Irisha::KILL));
	commands_.insert(std::pair<std::string, func>("ADMIN", &Irisha::ADMIN));
	commands_.insert(std::pair<std::string, func>("ERROR", &Irisha::ERROR));
	commands_.insert(std::pair<std::string, func>("MOTD", &Irisha::MOTD));
	commands_.insert(std::pair<std::string, func>("211", &Irisha::resend_msg));
	commands_.insert(std::pair<std::string, func>("219", &Irisha::resend_msg));
	commands_.insert(std::pair<std::string, func>("242", &Irisha::resend_msg));
	commands_.insert(std::pair<std::string, func>("256", &Irisha::RPL_256));
	commands_.insert(std::pair<std::string, func>("257", &Irisha::RPL_257));
	commands_.insert(std::pair<std::string, func>("258", &Irisha::RPL_258));
	commands_.insert(std::pair<std::string, func>("259", &Irisha::RPL_259));
	commands_.insert(std::pair<std::string, func>("421", &Irisha::RPL_421));
	commands_.insert(std::pair<std::string, func>("364", &Irisha::resend_msg));
	commands_.insert(std::pair<std::string, func>("365", &Irisha::resend_msg));
	commands_.insert(std::pair<std::string, func>("375", &Irisha::MOTD_REPLIES));
	commands_.insert(std::pair<std::string, func>("372", &Irisha::MOTD_REPLIES));
	commands_.insert(std::pair<std::string, func>("376", &Irisha::MOTD_REPLIES));
	commands_.insert(std::pair<std::string, func>("LUSERS", &Irisha::LUSERS));
	commands_.insert(std::pair<std::string, func>("SQUIT", &Irisha::SQUIT));
	commands_.insert(std::pair<std::string, func>("VERSION", &Irisha::VERSION));
	commands_.insert(std::pair<std::string, func>("351", &Irisha::VERSION));
	commands_.insert(std::pair<std::string, func>("005", &Irisha::resend_msg));
	commands_.insert(std::pair<std::string, func>("481", &Irisha::resend_msg));
	commands_.insert(std::pair<std::string, func>("CONNECT", &Irisha::CONNECT));
	commands_.insert(std::pair<std::string, func>("OPER", &Irisha::OPER));
	commands_.insert(std::pair<std::string, func>("STATS", &Irisha::STATS));
	commands_.insert(std::pair<std::string, func>("LINKS", &Irisha::LINKS));
	commands_.insert(std::pair<std::string, func>("ISON", &Irisha::ISON));
	commands_.insert(std::pair<std::string, func>("LUSERS_REPLIES", &Irisha::LUSERS_REPLIES));
}

eResult Irisha::ISON(const int sock)
{
	if (cmd_.arguments_.size() < 1)
	{
		err_needmoreparams(sock, "ISON");
		return R_FAILURE;
	}
	for (int i = 0; i < cmd_.arguments_.size(); i++)
	{
		User* user = find_user(cmd_.arguments_[i]);
		if (user != nullptr)
			rpl_ison(sock, user->nick());
	}
	return R_SUCCESS;
}

/**
 * Handles IRC LINKS command
 * show list of known servers or send this message to other server
 */
eResult Irisha::LINKS(const int sock)
{
	User* user = determine_user(sock);
	if (user == 0)
		return R_FAILURE;
	if (cmd_.arguments_.size() > 0 && cmd_.arguments_[0] != this->domain_)
	{
		//send message to other server
		Server* server = find_server(cmd_.arguments_[0]);
		if (server == 0)
		{
			err_nosuchserver(sock, cmd_.arguments_[0]);
			return R_FAILURE;
		}
		send_msg(choose_sock(server), user->nick(), cmd_.command_ + " " + cmd_.arguments_[0]);
		return R_SUCCESS;
	}

	std::map<std::string,AConnection*>::iterator it = connections_.begin();
	for (; it != connections_.end(); it++)
	{
		if (it->second->type() == T_SERVER)
		{
			Server* server = static_cast<Server*>(it->second);
			rpl_links(sock, server->name(), server->hopcount(), user->nick());
		}
	}
	rpl_endoflinks(sock, this->domain_, user->nick());
	return R_SUCCESS;
}

/**
 * Handles IRC STATS command
 * show statistic of this server or send STATS message to other server
 */
eResult Irisha::STATS(const int sock)
{
	User* user = determine_user(sock);
	if (user == 0 || !(user->is_operator()))
	{
		err_noprivileges(sock);
		return R_FAILURE;
	}

	if (cmd_.arguments_.size() == 2 && (cmd_.arguments_[1] != domain_))	//send to next server
	{
		Server* server = find_server(cmd_.arguments_[1]);
		if (server == 0)
		{
			err_nosuchserver(sock, cmd_.arguments_[1]);
			return R_FAILURE;
		}
		send_msg(choose_sock(server), user->nick(), cmd_.command_ + " " + cmd_.arguments_[0] + " " +
			cmd_.arguments_[1]);
		return R_SUCCESS;
	}

	if (cmd_.arguments_.size() == 0)
	{
		rpl_endofstats(sock, "*", user->nick());
		return R_SUCCESS;
	}

	if (cmd_.arguments_[0] == "l")
	{
		time_t cur_time = time(0);
		for(std::map<std::string, AConnection*>::iterator it = connections_.begin(); it != connections_.end(); it++)
		{
			if (it->second->socket() == U_EXTERNAL_CONNECTION)
				continue;
			std::string launch_time = double_to_str(difftime(cur_time, it->second->launch_time()));
			if (it->second->type() == T_SERVER)
			{
				Server *server = static_cast<Server*>(it->second);
				rpl_statslinkinfo(sock, server->name() + " " + launch_time + " sec ", user->nick());
			}
			else
			{
				User *client = static_cast<User*>(it->second);
				rpl_statslinkinfo(sock, user->nick() + "@" + client->host() + " " + launch_time + " sec ", user->nick());
			}
		}
	}
	else if (cmd_.arguments_[0] == "u")
	{
		std::string up_time = double_to_str(time(0) - this->launch_time_);
		rpl_statsuptime(sock, "Server up " + up_time + " sec", user->nick());
	}
	rpl_endofstats(sock, cmd_.arguments_[0], user->nick());
	return R_SUCCESS;
}

/**
 * Handles IRC CONNECT command
 * connect this server to other or send CONNECT message to other server
 */
eResult Irisha::CONNECT(const int sock)
{
	if (cmd_.arguments_.size() < 2)
	{
		err_needmoreparams(sock, "CONNECT");
		return R_FAILURE;
	}
	User *user = determine_user(sock);
	if (user == 0)
		return R_FAILURE;
	if (!user->is_operator())
	{
		err_noprivileges(sock);
		return R_FAILURE;
	}

	if (cmd_.arguments_.size() > 2 && cmd_.arguments_[2][0] == ':')
		cmd_.arguments_[2].erase(cmd_.arguments_[2].begin());
	if (cmd_.arguments_.size() == 2 ||
		(cmd_.arguments_.size() > 2 && cmd_.arguments_[2] == this->domain_)) //connect this server to other
	{
		try	{ connect_to_server(cmd_.arguments_[0], str_to_int(cmd_.arguments_[1])); }
		catch (std::runtime_error &ex)
		{
			err_nosuchserver(sock, cmd_.arguments_[0] + ":" + cmd_.arguments_[1]);
			return R_FAILURE;
		}
		//registration
		send_reg_info(password_);
		send_servers_info(parent_fd_);
		send_clients_info(parent_fd_);
		send_channels(parent_fd_);
	}
	else //send command to other server
	{
		AConnection* sender = find_connection(sock);
		Server* remote_serv = find_server(cmd_.arguments_[2]);
		if (remote_serv == 0)
		{
			err_nosuchserver(sock, cmd_.arguments_[2]);
			return R_FAILURE;
		}
		if (sender->type() == T_CLIENT)
			send_msg(choose_sock(remote_serv), static_cast<User*>(sender)->nick(), cmd_.line_);
		else
			send_msg(choose_sock(remote_serv), cmd_.line_);
	}
	return R_SUCCESS;
}

/**
 * Handles IRC VERSION command
 * send version info about this server or send VERSION message to the other server
 * @param sock socket
 * @return R_SUCCESS or R_FAILURE
 */
eResult Irisha::VERSION(const int sock)
{
	if (cmd_.command_ == "351") //reply
	{
		if (cmd_.arguments_.empty())
			return R_FAILURE;
		User* target = find_user(cmd_.arguments_[0]);
		if (target == 0)
			return R_FAILURE;
		send_msg(choose_sock(target), cmd_.line_);
		return R_SUCCESS;
	}

	AConnection* sender = find_connection(sock);
	if (sender == 0)
		return R_FAILURE;
	std::string nick;
	if (sender->type() == T_CLIENT)
		nick = static_cast<User*>(sender)->nick();
	else
		nick = cmd_.prefix_;

	//send info about current server
	if (cmd_.arguments_.size() == 0 || (cmd_.arguments_[0] == this->domain_))
	{
		rpl_version(sock, nick, "Irisha-1.0", "1", this->domain_, "");
		return R_SUCCESS;
	}
	//send query to other server
	Server*	target = find_server(cmd_.arguments_[0]);
	if (target == 0)
	{
		err_nosuchserver(sock, cmd_.arguments_[0]);
		return R_FAILURE;
	}
	send_msg(choose_sock(target), nick, "VERSION " + cmd_.arguments_[0]);
	return R_SUCCESS;
}

/**
 * Handles IRC OPER command
 * make user operator of the network
 * @param sock soket
 * @return R_SUCCESS or R_FAILURE
 */
eResult Irisha::OPER(const int sock)
{
	if (cmd_.arguments_.size() < 2)
	{
		err_needmoreparams(sock, cmd_.command_);
		return R_FAILURE;
	}
	if (oper_pass_.empty())
	{
		err_nooperhost(sock);
		return R_FAILURE;
	}
	User* sender = find_user(sock);
	if (sender == nullptr)
		return R_FAILURE;
	if (oper_pass_ == cmd_.arguments_[1])
	{
		User* user = find_user(cmd_.arguments_[0]);
		if (user == nullptr)
		{
			err_usersdontmatch(sock);
			return R_FAILURE;
		}
		if (user->is_operator())
			return R_SUCCESS;
		user->set_operator(true);
		user->set_mode_str('o');
		rpl_youreoper(choose_sock(user));
		//send msg to other servers to make user operator
		send_servers(sender->nick(), "MODE " + user->nick() + " +o");
		return R_SUCCESS;
	}
	else
	{
		err_passwdmismatch(sock);
		return R_FAILURE;
	}
	commands_.insert(std::pair<std::string, func>("NAMES", &Irisha::NAMES));
	commands_.insert(std::pair<std::string, func>("LIST", &Irisha::LIST));
	commands_.insert(std::pair<std::string, func>("INVITE", &Irisha::INVITE));
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
	send_servers(old_nick, "NICK " + new_nick);

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
	Server* serv = static_cast<Server*>(find_server(source_sock));
	send_servers(cmd_.prefix_, "NICK " + cmd_.arguments_[0] + " " + int_to_str(serv->hopcount() + 1) +
		" " + cmd_.arguments_[2] + " " + cmd_.arguments_[3] + " " + int_to_str(serv->token() + 1) +
		" " + cmd_.arguments_[5] + cmd_.arguments_[6], source_sock);

	if (user == nullptr)		// Add new external user
	{
		add_user(source_sock);
		//sending NICK msg to other servers
		return R_SUCCESS;
	}
	old_nick = user->nick();
	user->set_nick(new_nick);	// Change nick for external user
	sys_msg(E_GEAR, "User", old_nick, "changed nick to", new_nick);

	return R_SUCCESS;
}

/**
 * @description	Handles NICK command
 * @param		sock: command sender socket
 */
eResult	Irisha::NICK(const int sock)
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
	if (it == connections_.end()) // Add new local user
	{
		if (find_user(new_nick) != nullptr)
		{
			err_nicknameinuse(sock, new_nick);
			return R_FAILURE;
		}
		add_user(sock, new_nick);
	}
	else
	{
		User*	connection = dynamic_cast<User *>(it->second);
		if (connection == nullptr)	// Handle request from other server
			return NICK_server(new_nick, sock);
		return NICK_user(connection, sock, new_nick); // Change local user nickname
	}
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

	//set real name
	if (!cmd_.arguments_[3].empty() && cmd_.arguments_[3][0] == ':')
		cmd_.arguments_[3].erase(cmd_.arguments_[3].begin());
	else
		return R_FAILURE;
	user->set_realname(cmd_.arguments_[3]);

	if (cmd_.arguments_[1].length() > 1)
	{
		for (int i = 0; i < cmd_.arguments_[1].length(); i++)
		{
			if (cmd_.arguments_[1][i] == '8')
				user->set_mode_str('i');
		}
	}
	else
	{
		user->set_mode(str_to_int(cmd_.arguments_[1]));
		if (user->mode() == 8)
			user->set_mode_str('i');
	}
	rpl_welcome(sock);
	send_motd(sock);

	sys_msg(E_MAN, "New local user", user->nick(), "registered!");
	// NICK <nickname> <hopcount> <username> <host> <servertoken> <umode> <realname>
	//send_servers(domain_, "NICK " + user->nick() + " 1 " + user->username() + " " + user->server() + " 1 + " + user->realname());
	send_servers(domain_, createNICKmsg(user));
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
	{
		err_needmoreparams(sock, "PASS");
		return R_FAILURE;
	}
	else if (password_ == cmd_.arguments_[0] || !cmd_.prefix_.empty())
		return R_SUCCESS;
	else
	{
		send_msg(sock, domain_, "ERROR :Access denied! Bad password");
		return R_FAILURE;
	}
}

/**
 * @description	Handling SERVER message
 * @param		sock: command sender socket
 * @return		R_FAILURE if registration successfully, else R_FAILURE
 */
eResult Irisha::SERVER(const int sock)
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
			token = str_to_int(cmd_.arguments_[1]);
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
			hopcount = str_to_int(cmd_.arguments_[1]);
			token = str_to_int(cmd_.arguments_[2]);
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

			send_servers_info(sock);
			//send information about connected server to other servers
			std::map<std::string, AConnection*>::iterator it = connections_.begin();
			for (; it != connections_.end(); it++)
			{
				int cur_sock = it->second->socket();
				if (it->second->type() == T_SERVER && sock != cur_sock && cur_sock != U_EXTERNAL_CONNECTION)
					send_msg(cur_sock, domain_, createSERVERmsg(server));
			}
			send_clients_info(sock);
			send_channels(sock);
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
 * @return		PASS <password> <version> <flags>
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
 * @return		SERVER <servername> <hopcount> <token> <info>
 */
std::string Irisha::createSERVERmsg(AConnection* server) const
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

/**
 * @description	Returns NICK message string
 * @return		NICK <nickname> <hopcount> <username> <host> <servertoken> <umode> <realname>
 */
std::string Irisha::createNICKmsg(User* usr) const
{
	if (usr->socket() != U_EXTERNAL_CONNECTION) //local client
	{
		return ("NICK " + usr->nick() + " 1 " + usr->username() + " " +
								usr->host() + " 1 +" + usr->mode_str() + " :" + usr->realname());
	}
	else //remote client
	{
		return("NICK " + usr->nick() + " " + std::to_string(usr->hopcount() + 1) +
				 " " + usr->username() + " " + usr->host() + " " + std::to_string(usr->token() + 1) +
				 " +" + usr->mode_str() + " :" + usr->realname());
	}
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
	check_user(sock, user, cmd_.prefix_);

	std::string	msg = "user disconnected";
	if (user == nullptr)
	{
		sys_msg(E_CROSS, "Can't disconnect user", cmd_.prefix_, "(don't worry, not our problem)");
		return R_FAILURE;
	}
	sys_msg(E_SCULL, "User", user->nick(), "disconnected!");
	if (!cmd_.arguments_.empty())
		msg = cmd_.arguments_[0];

	if (user->socket() != U_EXTERNAL_CONNECTION) // If local user
	{
		send_servers(user->nick(), "QUIT " + msg);
		FD_CLR(sock, &all_fds_);
		FD_CLR(sock, &read_fds_);
		close(sock);
	}
	else
		send_servers(user->nick(), "QUIT " + msg, sock);
	remove_user(user->nick());
	return R_SUCCESS;
}

eResult Irisha::MODE(const int sock) // Доделать !!!
{
    std::string return_mode; // return str
    std::string std_params; // string param for return_mode
    std::string mode_flag = "aimnqpsrt";
    int         add_flag = 0; // flag add + or - to return_mode
    int         flag_mode = 2; // 1 on 0 off
    std::map<char, int>::const_iterator itr_m;
    std::list<std::string> arr_param; // array param for mode
    User* user;

    if (find_server(cmd_.prefix_)){
        if (channels_.find(cmd_.arguments_[0]) != channels_.end()){
            if (cmd_.arguments_.size() != 1){
                if ((cmd_.arguments_[0][0] == '#' || cmd_.arguments_[0][0] == '&' || cmd_.arguments_[0][0] == '+' || cmd_.arguments_[0][0] == '!')){
                    for (int i = 1; i < cmd_.arguments_[1].size(); ++i) {
                        channels_.find(cmd_.arguments_[0])->second->setMode(cmd_.arguments_[1][i], 1);
                    }
                }
                else
                {
                    for (int i = 0; i < cmd_.arguments_[1].size(); ++i) {
                        if (cmd_.arguments_[1][i] == '+'){
                            flag_mode = 1;
                            continue;
                        }
                        if (cmd_.arguments_[1][i] == '-'){
                            flag_mode = 0;
                            continue;
                        }
                        if (flag_mode == 1)
                            find_user(cmd_.arguments_[0])->set_mode_str(cmd_.arguments_[1][i]);
                        else
                            find_user(cmd_.arguments_[0])->del_mode_str(cmd_.arguments_[1][i]);
                    }
                }
                send_servers(cmd_.line_, sock);
            }
            return R_SUCCESS;
        }
    }
    if (check_user(sock, user, cmd_.prefix_) == R_FAILURE)
        return R_FAILURE;
//    if (!is_enough_args(user->socket(), cmd_.command_, 1))
//        return R_FAILURE;
    if ((cmd_.arguments_[0][0] == '#' || cmd_.arguments_[0][0] == '&' || cmd_.arguments_[0][0] == '+' || cmd_.arguments_[0][0] == '!')){ // it's channel ?
        std::map<std::string, Channel*>::iterator itr = channels_.find(cmd_.arguments_[0]);
        if (itr == channels_.end()){
            err_nosuchchannel(user->socket(), cmd_.arguments_[0]);
            return R_SUCCESS;
        }
        if (cmd_.arguments_.size() == 1) {
            send_msg(user->socket(), domain_, "324 " + user->nick() + " " + cmd_.arguments_[0] + " +" + (*itr).second->getListMode());
            return R_SUCCESS;
        }
        if (!(*itr).second->isOperator(user)){ // Error is operator
            send_msg(user->socket(), domain_, "482 " + user->nick() + " " + cmd_.arguments_[0] + " :You're not channel operator");
            return R_SUCCESS;
        }
        for (int i = 2; i < cmd_.arguments_.size(); ++i) {
            arr_param.push_back(cmd_.arguments_[i]);
        }
        for (size_t i = 0; i < cmd_.arguments_[1].size(); ++i) {
            if (cmd_.arguments_[1][i] == '+'){
                flag_mode = 1;
                continue;
            }
            if (cmd_.arguments_[1][i] == '-'){
                flag_mode = 0;
                continue;
            }
            if (flag_mode == 2)
                continue;
            itr_m = (*itr).second->getMode().find(cmd_.arguments_[1][i]);
            if (itr_m == (*itr).second->getMode().end()){ // Error char mode
                send_msg(user->socket(), domain_, "472 " + user->nick() + " " + cmd_.arguments_[1][i] + " :is unknown mode char to me");
                continue;
            }
            if (mode_flag.find(cmd_.arguments_[1][i]) != std::string::npos) { // flag mode ?
                if (flag_mode == 0 && (*itr).second->getMode().find(cmd_.arguments_[1][i])->second == 1) { // if on flag
                    (*itr).second->setMode(cmd_.arguments_[1][i], flag_mode);
                    if (add_flag == 0 || add_flag == 2) {
                        return_mode.push_back('-');
                        add_flag = 1;
                    }
                    return_mode.push_back(cmd_.arguments_[1][i]);
                } else if (flag_mode == 1 && (*itr).second->getMode().find(cmd_.arguments_[1][i])->second == 0) { // if off flag
                    (*itr).second->setMode(cmd_.arguments_[1][i], flag_mode);
                    if (add_flag == 0 || add_flag == 1) {                        return_mode.push_back('+');
                        add_flag = 2;
                    }
                    return_mode.push_back(cmd_.arguments_[1][i]);
                }
            } else {
                if (cmd_.arguments_[1][i] == 'o'){ // mode operator channel
                    if (arr_param.empty() || !is_a_valid_nick(arr_param.front())){ // empty param or nick not valid
                        if (!arr_param.empty())
                            arr_param.pop_front();
                        continue;
                    }
                    if (!(*itr).second->isUser(find_user(arr_param.front()))){ // not user in channel
                        send_msg(user->socket(), domain_, "441 " + user->nick() + " " + arr_param.front() + " " + cmd_.arguments_[0] + " :They aren't on that channel");
                        arr_param.pop_front();
                        continue;
                    }
                    if (flag_mode == 0){
                        (*itr).second->delOperators(find_user(arr_param.front()));
                        if (add_flag == 0 || add_flag == 2) {
                            return_mode.push_back('-');
                            add_flag = 1;
                        }
                        return_mode.push_back(cmd_.arguments_[1][i]);
                        std_params.append(arr_param.front() + " ");
                        arr_param.pop_front();
                    }
                    else if (flag_mode == 1){
                        (*itr).second->addOperators(find_user(arr_param.front()));
                        if (add_flag == 0 || add_flag == 1) {
                            return_mode.push_back('+');
                            add_flag = 2;
                        }
                        return_mode.push_back(cmd_.arguments_[1][i]);
                        std_params.append(arr_param.front() + " ");
                        arr_param.pop_front();
                    }
                }
                else if (cmd_.arguments_[1][i] == 'v'){ // mode voice
                    if (arr_param.empty() || !is_a_valid_nick(arr_param.front())){ // empty param or nick not valid
                        if (!arr_param.empty())
                            arr_param.pop_front();
                        continue;
                    }
                    if (!(*itr).second->isUser(find_user(arr_param.front()))){ // not user in channel
                        send_msg(user->socket(), domain_, "441 " + user->nick() + " " + arr_param.front() + " " + cmd_.arguments_[0] + " :They aren't on that channel");
                        arr_param.pop_front();
                        continue;
                    }
                    if (flag_mode == 0){
                        (*itr).second->delModeratorUser(find_user(arr_param.front()));
                        if (add_flag == 0 || add_flag == 2) {
                            return_mode.push_back('-');
                            add_flag = 1;
                        }
                        return_mode.push_back(cmd_.arguments_[1][i]);
                        std_params.append(arr_param.front() + " ");
                        arr_param.pop_front();
                    }
                    else if (flag_mode == 1){
                        (*itr).second->addModeratorUser(find_user(arr_param.front()));
                        if (add_flag == 0 || add_flag == 1) {
                            return_mode.push_back('+');
                            add_flag = 2;
                        }
                        return_mode.push_back(cmd_.arguments_[1][i]);
                        std_params.append(arr_param.front() + " ");
                        arr_param.pop_front();
                    }
                }
            }
        }
        if (!std_params.empty()){
            std_params.pop_back();
            return_mode.append(" " + std_params);
        }
        if (cmd_.type_ == T_LOCAL_CLIENT)
            send_msg(user->socket(), user->nick(), "MODE " + cmd_.arguments_[0] + " " + return_mode);
        send_channel((*itr).second, "MODE " + cmd_.arguments_[0] + " " + return_mode, user->nick(), sock);
    }
    else{ // user mode
        if (find_user(cmd_.arguments_[0]) == nullptr){
            err_nosuchchannel(user->socket(), cmd_.arguments_[0]);
            return R_SUCCESS;
        }
        if (user->nick() != cmd_.arguments_[0]){
            send_msg(user->socket(), domain_, "502 :Cant change mode for other users");
            return R_SUCCESS;
        }
        if (cmd_.arguments_.size() == 1){
                if (!user->mode_str().empty())
                    send_msg(user->socket(), domain_, "221 " + user->nick() + " +" + user->mode_str());
                return R_SUCCESS;
        }
        for (size_t i = 0; i < cmd_.arguments_[1].size(); ++i) {
            if (cmd_.arguments_[1][i] == '+') {
                flag_mode = 1;
                continue;
            }
            if (cmd_.arguments_[1][i] == '-') {
                flag_mode = 0;
                continue;
            }
            if (flag_mode == 2)
                continue;
            if (flag_mode == 1 && cmd_.arguments_[1][i] == 'o' && find_server(sock) == nullptr)
                continue;
            if (cmd_.arguments_[1][i] == 'o'){
                if (flag_mode == 1){
                    if (user->mode_str().find(cmd_.arguments_[1][i]) == std::string::npos){
                        user->set_mode_str('o');
                        if (add_flag == 0 || add_flag == 1) {
                            return_mode.push_back('+');
                            add_flag = 2;
                        }
                        return_mode.push_back(cmd_.arguments_[1][i]);
                    }
                } else {
                    if (user->mode_str().find(cmd_.arguments_[1][i]) != std::string::npos){
                        user->del_mode_str('o');
                        if (add_flag == 0 || add_flag == 2) {
                            return_mode.push_back('-');
                            add_flag = 1;
                        }
                        return_mode.push_back(cmd_.arguments_[1][i]);
                    }
                }
                continue;
            }
            if (cmd_.arguments_[1][i] == 'i'){
                if (flag_mode == 1){
                    if (user->mode_str().find(cmd_.arguments_[1][i]) == std::string::npos){
                        user->set_mode_str('i');
                        if (add_flag == 0 || add_flag == 1) {
                            return_mode.push_back('+');
                            add_flag = 2;
                        }
                        return_mode.push_back(cmd_.arguments_[1][i]);
                    }
                } else {
                    if (user->mode_str().find(cmd_.arguments_[1][i]) != std::string::npos){
                        user->del_mode_str('i');
                        if (add_flag == 0 || add_flag == 2) {
                            return_mode.push_back('-');
                            add_flag = 1;
                        }
                        return_mode.push_back(cmd_.arguments_[1][i]);
                    }
                }
                continue;
            }
        }
        if (return_mode.empty())
            return R_SUCCESS;
        if (cmd_.type_ == T_LOCAL_CLIENT)
            send_msg(user->socket(), user->nick(), "MODE " + cmd_.arguments_[0] + " " + return_mode);
        send_servers(user->nick(), "MODE " + cmd_.arguments_[0] + " " + return_mode, sock);
    }
    return R_SUCCESS;
}

int     Irisha::check_mode_channel(const Channel* channel, const int sock, std::list<std::string>& arr_key, std::string& arr_channel)
{
    CITERATOR itr = channel->getBanUsers().begin();
    CITERATOR ite = channel->getBanUsers().end();
    if (!channel->getKey().empty()){
        if (arr_key.empty() || arr_key.front() != channel->getKey()) {
            send_msg(sock, domain_, "475 " + arr_channel + " :Cannot join channel (+k)");
            arr_key.pop_front();
            return 1;
        }
        arr_key.pop_front();
    }
    while (itr != ite)
    {
        if ((*itr) == find_user(sock)){
            send_msg(sock, domain_, "474 " + arr_channel + " :Cannot join channel (+b)");
            return 1;
        }
        itr++;
    }
    if (channel->getMode().find('i')->second == 1){
        itr = channel->getInviteUsers().begin();
        ite = channel->getInviteUsers().end();
        while (itr != ite){
            if ((*itr) == find_user(sock))
                break;
            itr++;
        }
        if (itr == ite){
            send_msg(sock, domain_, "473 " + arr_channel + " :Cannot join channel (+i)");
            return 1;
        }
    }
    if (channel->getMode().find('l')->second == 1){
        if (channel->getUsers().size() >= channel->getMaxUsers()){
            send_msg(sock, domain_, "471 " + arr_channel + " :Cannot join channel (+l)");
            return 1;
        }
    }
    return 0;
}

eResult Irisha::JOIN(const int sock)
{
    std::vector<std::string> arr_channel; // array channels
    std::list<std::string> arr_key; // arrray key for channels
    std::string str_channels = cmd_.arguments_[0]; // string channels "#123,#321"

    User* user;
    if (check_user(sock, user, cmd_.prefix_) == R_FAILURE)
        return R_FAILURE;
    if (!is_enough_args(user->socket(), cmd_.command_, 1))
        return R_FAILURE;
    if (cmd_.arguments_.size() == 2){
        std::string str_keys = cmd_.arguments_[1]; // string keys "hello,world"
        parse_arr_list(arr_key, str_keys, ',');
    }
    parse_arr(arr_channel, str_channels, ',');
    for (size_t i = 0; i < arr_channel.size(); ++i) {
        if ((arr_channel[i][0] == '#' || arr_channel[i][0] == '&' || arr_channel[i][0] == '+' || arr_channel[i][0] == '!')){
            std::map<std::string, Channel*>::iterator itr = channels_.find(arr_channel[i]);
            if (itr == channels_.end()){
                if (arr_channel[i].size() > 50){
                    err_nosuchchannel(sock, arr_channel[i]);
                    continue;
                }
                //send_msg(sock, "", ":" + user->nick() + " JOIN " + arr_channel[i]);
                Channel* channel = new Channel(arr_channel[i]);
                channel->setType(arr_channel[i][0]);
                channel->addOperators(user);
                channel->addUser(user);
                user->set_channel(arr_channel[i]);
                if (!arr_key.empty())
                    channel->setKey(arr_key.front());
                channels_.insert(std::pair<std::string, Channel*>(arr_channel[i], channel));
                if (cmd_.type_ == T_LOCAL_CLIENT){
                    send_msg(user->socket(), "", ":" + user->nick() + " JOIN " + arr_channel[i]);
                    send_msg(user->socket(), domain_, "353 " + user->nick() + " = " + arr_channel[i] +" :" + channel->getListUsers());
                    send_msg(user->socket(), domain_, "366 " + user->nick() + " " + arr_channel[i] +" :End of NAMES list");
                    send_servers(user->nick(), "JOIN " + arr_channel[i]);
                }
                else
                    send_servers(user->nick(), "JOIN " + arr_channel[i], sock);
            }
            else{
				if (check_mode_channel((*itr).second, sock, arr_key, arr_channel[i]) == 1)
					continue;
				itr->second->addUser(user);
                user->set_channel(arr_channel[i]);
                if (cmd_.type_ == T_LOCAL_CLIENT) {
                    send_msg(user->socket(), "", ":" + user->nick() + " JOIN " + arr_channel[i]);
                    if (itr->second->getTopic().empty())
                        send_msg(user->socket(), domain_, "331 " + user->nick() + " " + arr_channel[i] + " :No topic is set");
                    else
                        send_msg(user->socket(), domain_,
                                 "332 " + user->nick() + " " + arr_channel[i] + " :" + itr->second->getTopic());
                    send_msg(user->socket(), domain_,
                             "353 " + user->nick() + " = " + arr_channel[i] + " :" + itr->second->getListUsers());
                    send_msg(user->socket(), domain_, "366 " + user->nick() + " " + arr_channel[i] + " :End of NAMES list");

                }
                send_channel((*itr).second, "JOIN " + arr_channel[i], user->nick(), sock);
            }
        }
    }
    return R_SUCCESS;
}

eResult Irisha::NJOIN(const int sock)
{
    std::vector<std::string> arr_users;

    if (cmd_.arguments_.size() == 0)
        return R_SUCCESS;
    cmd_.arguments_[1].erase(cmd_.arguments_[1].begin());
    parse_arr(arr_users, cmd_.arguments_[1], ',');
    std::map<std::string, Channel*>::iterator itr = channels_.find(cmd_.arguments_[0]);
    if (itr == channels_.end())
    {
        Channel* channel = new Channel(cmd_.arguments_[0]);
        channel->setType(cmd_.arguments_[0][0]);
        for (int i = 0; i < arr_users.size(); ++i) {
            if (arr_users[i][0] == '@'){
                arr_users[i].erase(arr_users[i].begin());
                channel->addOperators(find_user(arr_users[i]));
            }
            else if (arr_users[i][0] == '+'){
                arr_users[i].erase(arr_users[i].begin());
                channel->addModeratorUser(find_user(arr_users[i]));
            }
            channel->addUser(find_user(arr_users[i]));
            find_user(arr_users[i])->set_channel(cmd_.arguments_[0]);
        }
        channels_.insert(std::pair<std::string, Channel*>(cmd_.arguments_[0], channel));
    }
    return R_SUCCESS;
}

eResult Irisha::PART(const int sock)
{
    std::vector<std::string> arr_channel;
    std::string str_channels = cmd_.arguments_[0];
    User* user;

    if (check_user(sock, user, cmd_.prefix_) == R_FAILURE)
        return R_FAILURE;
//    if (!is_enough_args(user->socket(), cmd_.command_, 1))
//        return R_FAILURE;
    parse_arr(arr_channel, str_channels, ',');
    for (size_t i = 0; i < arr_channel.size(); ++i) {
        if ((arr_channel[i][0] == '#' || arr_channel[i][0] == '&' || arr_channel[i][0] == '+' || arr_channel[i][0] == '!')){
            std::map<std::string, Channel*>::iterator itr = channels_.find(arr_channel[i]);
            if (cmd_.type_ == T_LOCAL_CLIENT) {
                if (itr == channels_.end()) {
                    err_nosuchchannel(user->socket(), arr_channel[i]);
                    continue;
                }
                if (!(*itr).second->isUser(user)) {
                    send_msg(user->socket(), domain_,
                             "442 " + user->nick() + " " + arr_channel[i] + " :You're not on that channel");
                    continue;
                }
                send_channel((*itr).second, "PART " + arr_channel[i], user->nick());
                (*itr).second->delUser(user);
                (*itr).second->delOperators(user);
                user->del_channel(arr_channel[i]);
                if ((*itr).second->getUsers().size() == 0) {
                    channels_.erase(itr);
                    delete ((*itr).second);
                    //return R_SUCCESS;
                }
            }
            else{
                send_channel((*itr).second, "PART " + arr_channel[i], user->nick(), choose_sock(user));
                (*itr).second->delUser(user);
                (*itr).second->delOperators(user);
                user->del_channel(arr_channel[i]);
                if ((*itr).second->getUsers().size() == 0) {
                    delete ((*itr).second);
                    channels_.erase(itr);
                    //return R_SUCCESS;
                }

            }
        }
    }
    return R_SUCCESS;
}

eResult Irisha::TOPIC(const int sock)
{
    User* user;

    if (find_server(cmd_.prefix_)){
        if (channels_.find(cmd_.arguments_[0]) != channels_.end()){
            if (cmd_.arguments_.size() != 1)
                channels_.find(cmd_.arguments_[0])->second->setTopic(cmd_.arguments_[1]);
            return R_SUCCESS;
        }
    }
    if (check_user(sock, user, cmd_.prefix_) == R_FAILURE)
        return R_FAILURE;

    if (cmd_.arguments_.size() > 2)
        return R_SUCCESS;
    if ((cmd_.arguments_[0][0] == '#' || cmd_.arguments_[0][0] == '&' || cmd_.arguments_[0][0] == '+' || cmd_.arguments_[0][0] == '!')){
        std::map<std::string, Channel*>::iterator itr = channels_.find(cmd_.arguments_[0]);
        if (itr == channels_.end()){
            err_nosuchchannel(user->socket(), cmd_.arguments_[0]);
            return R_SUCCESS;
        }
        if (cmd_.arguments_.size() == 1){
            if (itr->second->getMode().find('p')->second == 1 || itr->second->getMode().find('s')->second == 1){
                send_msg(user->socket(), domain_, "442 " + user->nick() + " " + cmd_.arguments_[0] + " :You're not on that channel");
                return R_SUCCESS;
            }
            if (itr->second->getTopic().empty())
                send_msg(user->socket(), domain_, "331 " + user->nick() + " " + cmd_.arguments_[0] + " :No topic is set");
            else
                send_msg(user->socket(), domain_, "332 " + user->nick() + " " + cmd_.arguments_[0] + " :" + itr->second->getTopic());
            return R_SUCCESS;
        }
        if (!(*itr).second->isUser(user)){
            send_msg(user->socket(), domain_, "442 " + user->nick() + " " + cmd_.arguments_[0] + " :You're not on that channel");
            return R_SUCCESS;
        }
        if (!(*itr).second->isOperator(user)){
            send_msg(user->socket(), domain_, "482 " + user->nick() + " " + cmd_.arguments_[0] + " :You're not channel operator");
            return R_SUCCESS;
        }
        if ((*itr).second->getMode().find('t')->second == 0){
            send_msg(user->socket(), domain_, "477 " + user->nick() + " " + cmd_.arguments_[0] + " :Channel doesn't support modes");
            return R_SUCCESS;
        }
        cmd_.arguments_[1].erase(cmd_.arguments_[1].begin());
        if (cmd_.arguments_[1] == (*itr).second->getTopic())
            return R_SUCCESS;
        (*itr).second->setTopic(cmd_.arguments_[1]);
        send_channel((*itr).second, "TOPIC " + cmd_.arguments_[0] + " :" + cmd_.arguments_[1], user->nick());
    }
    return R_SUCCESS;
}

eResult Irisha::PRIVMSG(const int sock)
{
    User* sender;
    if (check_user(sock, sender, cmd_.prefix_) == R_FAILURE)
        return R_FAILURE;

    if (cmd_.arguments_.size() == 0){
        send_msg(sender->socket(), domain_, "411 " + sender->nick() + " " + " :No recipient given (PRIVMSG)");
        return R_SUCCESS;
    }
    if (cmd_.arguments_.size() == 1){
        send_msg(sender->socket(), domain_, "412 " + sender->nick() + " " + " :No text to send");
        return R_SUCCESS;
    }

    std::list<std::string> arr_receiver;
    std::string str_receiver = cmd_.arguments_[0];
    User* user;

    parse_arr_list(arr_receiver, str_receiver, ',');
    arr_receiver.sort();
    arr_receiver.unique();
    while (!arr_receiver.empty()){
        if ((arr_receiver.front()[0] == '#' || arr_receiver.front()[0] == '&' || arr_receiver.front()[0] == '+' || arr_receiver.front()[0] == '!')){
            std::map<std::string, Channel*>::iterator itr = channels_.find(arr_receiver.front());
            if (itr == channels_.end()){
                err_nosuchnick(sender->socket(), arr_receiver.front());
                arr_receiver.pop_front();
                continue;
            }
            if (!(*itr).second->isUser(sender) && (*itr).second->getMode().find('n')->second == 1){
                send_msg(sender->socket(), domain_, "404 " + sender->nick() + " " + arr_receiver.front() + " :Cannot send to channel");
                arr_receiver.pop_front();
                continue;
            }
            if (!(*itr).second->isModerator(sender) && !(*itr).second->isOperator(sender) && (*itr).second->getMode().find('m')->second == 1){
                send_msg(sender->socket(), domain_, "404 " + sender->nick() + " " + arr_receiver.front() + " :Cannot send to channel");
                arr_receiver.pop_front();
                continue;
            }
            if (cmd_.type_ == T_LOCAL_CLIENT)
                send_channel((*itr).second, "PRIVMSG " + arr_receiver.front() + " " + cmd_.arguments_[1], sender->nick(), sock);
            else
                send_channel((*itr).second, "PRIVMSG " + arr_receiver.front() + " " + cmd_.arguments_[1], sender->nick(), choose_sock(sender));
        } else {
			user = find_user(arr_receiver.front());
			if (user == nullptr)
			{
				err_nosuchnick(sock, arr_receiver.front());
				arr_receiver.pop_front();
				continue;
			}
			send_msg(choose_sock(user), sender->nick(), "PRIVMSG " + arr_receiver.front() + " " + cmd_.arguments_[1]);
        }
        arr_receiver.pop_front();
    }
    return R_SUCCESS;
}

//ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
//ERR_WILDTOPLEVEL

eResult Irisha::NOTICE(const int sock)
{
    User* sender;
    if (check_user(sock, sender, cmd_.prefix_) == R_FAILURE)
        return R_FAILURE;
    std::list<std::string> arr_receiver;
    std::string str_receiver = cmd_.arguments_[0];
    User* user;

    parse_arr_list(arr_receiver, str_receiver, ',');
    arr_receiver.sort();
    arr_receiver.unique();
    while (!arr_receiver.empty()){
        if ((arr_receiver.front()[0] == '#' || arr_receiver.front()[0] == '&' || arr_receiver.front()[0] == '+' || arr_receiver.front()[0] == '!')){
            std::map<std::string, Channel*>::iterator itr = channels_.find(arr_receiver.front());
            if (itr == channels_.end()){
                arr_receiver.pop_front();
                continue;
            }
            if (!(*itr).second->isUser(sender) && (*itr).second->getMode().find('n')->second == 1){
                arr_receiver.pop_front();
                continue;
            }
            if (!(*itr).second->isModerator(sender) && !(*itr).second->isOperator(sender) && (*itr).second->getMode().find('m')->second == 1){
                arr_receiver.pop_front();
                continue;
            }
            if (cmd_.type_ == T_LOCAL_CLIENT)
                send_channel((*itr).second, "PRIVMSG " + arr_receiver.front() + " " + cmd_.arguments_[1], sender->nick(), sock);
            else
                send_channel((*itr).second, "PRIVMSG " + arr_receiver.front() + " " + cmd_.arguments_[1], sender->nick(), choose_sock(sender));
        } else {
			user = find_user(arr_receiver.front());
			if (user == nullptr)
			{
				err_nosuchnick(sock, arr_receiver.front());
				arr_receiver.pop_front();
				continue;
			}
			send_msg(choose_sock(user), sender->nick(), "PRIVMSG " + arr_receiver.front() + " " + cmd_.arguments_[1]);
        }
        arr_receiver.pop_front();
    }
    return R_SUCCESS;
}

eResult Irisha::NAMES(const int sock) // list users channels
{
    std::vector<std::string> arr_channel;
    std::string str_channels = cmd_.arguments_[0];
    User* user;

    if (check_user(sock, user, cmd_.prefix_) == R_FAILURE)
        return R_FAILURE;

    parse_arr(arr_channel, str_channels, ',');
    for (size_t i = 0; i < arr_channel.size(); ++i) {
        if ((arr_channel[i][0] == '#' || arr_channel[i][0] == '&' || arr_channel[i][0] == '+' || arr_channel[i][0] == '!')) {
            std::map<std::string, Channel *>::iterator itr = channels_.find(arr_channel[i]);
            if (itr == channels_.end())
                continue;
            if (itr->second->getMode().find('s')->second == 1 && !itr->second->isUser(find_user(sock)))
                    continue;
            send_msg(user->socket(), domain_, "353 " + user->nick() + " = " + arr_channel[i] +" :" + itr->second->getListUsers());
            send_msg(user->socket(), domain_, "366 " + user->nick() + " " + arr_channel[i] +" :End of NAMES list");
        }
    }
    return R_SUCCESS;
}

eResult Irisha::LIST(const int sock) // list server ?
{
    std::map<std::string ,Channel*>::iterator itr = channels_.begin();
    std::map<std::string ,Channel*>::iterator ite = channels_.end();
    User* user;

    if (check_user(sock, user, cmd_.prefix_) == R_FAILURE)
        return R_FAILURE;
    while (itr != ite){
        if (itr->second->getMode().find('s')->second == 1 && !itr->second->isUser(user)){
            itr++;
            continue;
        }
        send_msg(user->socket(), domain_, "322 " + user->nick() + " " + itr->second->getName() + " " + int_to_str(itr->second->getUsers().size()) + " :" + itr->second->getTopic());
        itr++;
    }
    send_msg(user->socket(), domain_, "323 " + user->nick() + " :End of /LIST");
    return R_SUCCESS;
}

eResult Irisha::INVITE(const int sock) {
    std::map<std::string, Channel *>::iterator itr = channels_.find(cmd_.arguments_[1]);
    User* user;

    if (check_user(sock, user, cmd_.prefix_) == R_FAILURE)
        return R_FAILURE;

    if (find_user(cmd_.arguments_[0]) == nullptr) {
        send_msg(sock, domain_, "401 " + find_user(sock)->nick() + " " + cmd_.arguments_[0] + " :No such nick/channel");
        return R_SUCCESS;
    }
    if (!itr->second->isUser(user)) {
        send_msg(sock, domain_,
                 "442 " + user->nick() + " " + cmd_.arguments_[1] + " :You're not on that channel");
        return R_SUCCESS;
    }
    if (itr->second->isUser(find_user(cmd_.arguments_[0]))) {
        send_msg(sock, domain_, "443 " + user->nick() + " " + cmd_.arguments_[0] + " " + cmd_.arguments_[1] +
                                " :is already on channel");
        return R_SUCCESS;
    }
    if (itr->second->getMode().find('i')->second == 1 && !itr->second->isOperator(user)) {
        send_msg(sock, domain_,
                 "482 " + user->nick() + " " + cmd_.arguments_[1] + " :You're not channel operator");
        return R_SUCCESS;
    }
    rpl_inviting(sock, cmd_.arguments_[1], cmd_.arguments_[0]);
    itr->second->addInviteUser(find_user(cmd_.arguments_[0]));
    return R_SUCCESS;
}

eResult Irisha::KICK(const int sock)
{
    User* user;
    User* sender;

    if (check_user_sender(sock, sender, cmd_.prefix_, user, cmd_.arguments_[1]) == R_FAILURE)
        return R_FAILURE;
    if (!is_enough_args(sock, cmd_.command_, 3))
        return R_FAILURE;
    std::map<std::string, Channel *>::iterator itr = channels_.find(cmd_.arguments_[0]);
    if (itr == channels_.end()){
        err_nosuchchannel(sock, cmd_.arguments_[0]);
        return R_SUCCESS;
    }
    if (!itr->second->isOperator(sender)){
        err_chanoprivsneeded(sock, cmd_.arguments_[0]);
        return R_SUCCESS;
    }
    if (!itr->second->isUser(user)){
        err_notochannel(sock, cmd_.arguments_[0]);
        return R_SUCCESS;
    }
    (*itr).second->delUser(user);
    if (user->socket() != U_EXTERNAL_CONNECTION)
        send_msg(user->socket(), sender->nick(), "KICK " + cmd_.arguments_[0] + " " + cmd_.arguments_[1] + " " + cmd_.arguments_[2]);
    if (sender->socket() != U_EXTERNAL_CONNECTION)
        send_msg(sock, sender->nick(), "KICK " + cmd_.arguments_[0] + " " + cmd_.arguments_[1] + " " + cmd_.arguments_[2]);
//        send_msg(user->socket(), sender->nick(), "KICK " + cmd_.arguments_[0] + " " + cmd_.arguments_[1] + " " + cmd_.arguments_[2]);
    send_channel((*itr).second, "KICK " + cmd_.arguments_[0] + " " + cmd_.arguments_[1] + " " + cmd_.arguments_[2], sender->nick(), sock);
//    }
//    else
//        send_servers(sender->nick(), "KICK " + cmd_.arguments_[0] + " " + cmd_.arguments_[1] + " " + cmd_.arguments_[2], sock);
    return R_SUCCESS;
}
//:WiZ KICK #Finnish John
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
	AConnection* killer = find_connection(cmd_.prefix_);
	if (killer == nullptr)
		killer = find_connection(sock);
	if (killer == nullptr || (killer->type() == T_CLIENT && !static_cast<User*>(killer)->is_operator()))
	{
		err_noprivileges(sock);
		return R_FAILURE;
	}

	std::string msg(cmd_.arguments_[1].begin() + 1, cmd_.arguments_[1].end());
	AConnection* victim = find_connection(cmd_.arguments_[0]);
	if (victim == nullptr)
		err_nosuchnick(sock, cmd_.arguments_[0]);
	else if (victim->type() == T_SERVER)
		err_cantkillserver(sock);
	else if (victim->socket() != U_EXTERNAL_CONNECTION)
	{
		send_msg(victim->socket(), connection_name(killer), "KILL "
				+ cmd_.arguments_[0] + " :KILLed by " + connection_name(killer) + ": " + msg);
		send_msg(victim->socket(), domain_, "ERROR :Killed by " + connection_name(killer) + ": " + msg);
		close_connection(victim->socket(), "Killed by " + connection_name(killer), nullptr);
		return R_SUCCESS;
	}
	else
	{
		send_msg(choose_sock(victim), connection_name(killer), "KILL "
					+ cmd_.arguments_[0] + " :KILLed by " + connection_name(killer) + ": " + msg);
		return R_SUCCESS;
	}
	return R_FAILURE;
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
	std::string prefix = cmd_.prefix_;
	if (prefix.empty())
		prefix = connection_name(sock);
	for (int i = 0; i < cmd_.arguments_.size(); ++i)
	{
		server = find_server(cmd_.arguments_[i]);
		if (cmd_.arguments_[i] == domain_)
			admin_info(sock, prefix);
		else if (server == nullptr)
			err_nosuchserver(sock, cmd_.arguments_[i]);
		else
			send_msg(choose_sock(server), prefix, "ADMIN " + cmd_.arguments_[i]);
	}
	return R_SUCCESS;
}


eResult Irisha::ERROR(const int sock)
{
	std::cout << E_CROSS RED " ALARM! " + cmd_.command_ + " " + cmd_.arguments_[0] + " " E_CROSS CLR << std::endl;
	return R_SUCCESS;
}

eResult Irisha::RPL_256(const int sock)
{
	AConnection*	server = find_server(sock);
	if (server == nullptr || cmd_.arguments_.size() < 3)
		return R_FAILURE;
	AConnection*	user = find_user(cmd_.arguments_[0]);
	if (user == nullptr)
		return R_FAILURE;
	send_msg(choose_sock(user), domain_, cmd_.command_ + " " + cmd_.arguments_[0]
						+ " " + cmd_.arguments_[1] + " " + cmd_.arguments_[2]);
	return R_SUCCESS;
}

eResult Irisha::RPL_257(const int sock)
{
	AConnection*	server = find_server(sock);
	if (server == nullptr || cmd_.arguments_.size() < 2)
		return R_FAILURE;
	AConnection*	user = find_user(cmd_.arguments_[0]);
	if (user == nullptr)
		return R_FAILURE;
	send_msg(choose_sock(user), domain_, cmd_.command_ + " " + cmd_.arguments_[0]
						+ " " + cmd_.arguments_[1]);
	return R_SUCCESS;
}

eResult Irisha::RPL_258(const int sock)
{
	AConnection*	server = find_server(sock);
	if (server == nullptr || cmd_.arguments_.size() < 2)
		return R_FAILURE;
	AConnection*	user = find_user(cmd_.arguments_[0]);
	if (user == nullptr)
		return R_FAILURE;
	send_msg(choose_sock(user), domain_, cmd_.command_ + " " + cmd_.arguments_[0]
										 + " " + cmd_.arguments_[1]);
	return R_SUCCESS;
}

eResult Irisha::RPL_259(const int sock)
{
	AConnection*	server = find_server(sock);
	if (server == nullptr || cmd_.arguments_.size() < 2)
		return R_FAILURE;
	AConnection*	user = find_user(cmd_.arguments_[0]);
	if (user == nullptr)
		return R_FAILURE;
	send_msg(choose_sock(user), domain_, cmd_.command_ + " " + cmd_.arguments_[0]
										 + " " + cmd_.arguments_[1]);
	return R_SUCCESS;
}

/**
 * @description	Does nothing for unknown command reply (just for server safety of infinite loops)
 * @param		sock
 * @return		R_SUCCESS
 */
eResult Irisha::RPL_421(const int sock)
{
    return R_SUCCESS;
}

void Irisha::send_motd(const int sock)
{
	rpl_motdstart(sock, domain_);
	rpl_motd(sock, " __      __          ___                                          __");
	rpl_motd(sock, "/\\ \\  __/\\ \\        /\\_ \\                                        /\\ \\__");
	rpl_motd(sock, "\\ \\ \\/\\ \\ \\ \\     __\\//\\ \\     ___    ___     ___ ___      __    \\ \\ ,_\\   ___");
	rpl_motd(sock, " \\ \\ \\ \\ \\ \\ \\  /'__`\\\\ \\ \\   /'___\\ / __`\\ /' __` __`\\  /'__`\\   \\ \\ \\/  / __`\\");
	rpl_motd(sock, "  \\ \\ \\_/ \\_\\ \\/\\  __/ \\_\\ \\_/\\ \\__//\\ \\L\\ \\/\\ \\/\\ \\/\\ \\/\\  __/    \\ \\ \\_/\\ \\L\\ \\");
	rpl_motd(sock, "   \\ `\\___x___/\\ \\____\\/\\____\\ \\____\\ \\____/\\ \\_\\ \\_\\ \\_\\ \\____\\    \\ \\__\\ \\____/");
	rpl_motd(sock, "    '\\/__//__/  \\/____/\\/____/\\/____/\\/___/  \\/_/\\/_/\\/_/\\/____/     \\/__/\\/___/");
	rpl_motd(sock, "");
	rpl_motd(sock, "");
	rpl_motd(sock, " __ __  ______   ____    ______  ____    __  __  ______      ______   ____    ____     __ __");
	rpl_motd(sock, "/\\ \\\\ \\/\\__  _\\ /\\  _`\\ /\\__  _\\/\\  _`\\ /\\ \\/\\ \\/\\  _  \\    /\\__  _\\ /\\  _`\\ /\\  _`\\  /\\ \\\\ \\");
	rpl_motd(sock, "\\ \\_\\\\_\\/_/\\ \\/ \\ \\ \\L\\ \\/_/\\ \\/\\ \\,\\L\\_\\ \\ \\_\\ \\ \\ \\L\\ \\   \\/_/\\ \\/ \\ \\ \\L\\ \\ \\ \\/\\_\\\\ \\_\\\\_\\");
	rpl_motd(sock, " \\/_//_/  \\ \\ \\  \\ \\ ,  /  \\ \\ \\ \\/_\\__ \\\\ \\  _  \\ \\  __ \\     \\ \\ \\  \\ \\ ,  /\\ \\ \\/_/_\\/_//_/");
	rpl_motd(sock, "           \\_\\ \\__\\ \\ \\\\ \\  \\_\\ \\__/\\ \\L\\ \\ \\ \\ \\ \\ \\ \\/\\ \\     \\_\\ \\__\\ \\ \\\\ \\\\ \\ \\L\\ \\");
	rpl_motd(sock, "           /\\_____\\\\ \\_\\ \\_\\/\\_____\\ `\\____\\ \\_\\ \\_\\ \\_\\ \\_\\    /\\_____\\\\ \\_\\ \\_\\ \\____/");
	rpl_motd(sock, "           \\/_____/ \\/_/\\/ /\\/_____/\\/_____/\\/_/\\/_/\\/_/\\/_/    \\/_____/ \\/_/\\/ /\\/___/");
	rpl_motd(sock, "");
	rpl_motd(sock, "");
	rpl_motd(sock, "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                              @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
	rpl_motd(sock, "  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
	rpl_motd(sock, "	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@(          @@@@@@@@&                 /@@@@@@@@@          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
	rpl_motd(sock, "	  @@@.                                   @@@@@@@@@@@@@@@@@@@@@&          (@@@  @@@@@@               @@@@@@@@@@@@           @@@@@@@@@@@@@@@@@@@@@*                                   #@@@");
	rpl_motd(sock, "				*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ @@@@@@           @@@@  @@@@@ .@@@         @@@@  @@@@@  @@@@          @@@@@@@ @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
	rpl_motd(sock, "		  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@      /@@@@@@@@@@@@@         @@@@         @@@@@@@@#  @@@@@@@@@         @@@@         @@@@@@@@@@@@@       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
	rpl_motd(sock, "			@@@@@@@@@@@@@@@@@@@,          /@@@@@@@@@@@@@@@@@@@@@@           @           @@@@@@@    @@@@@@                        @@@@@@@@@@@@@@@@@@@@@@           @@@@@@@@@@@@@@@@@@@");
	rpl_motd(sock, "			   @@@               ,@@@@@@@@@@@@@@@@@@@@@@  @@@@@@@@                    @   @@@@  @@  @@@   @,                    @@@@@@@   @@@@@@@@@@@@@@@@@@@@@@               %@@@");
	rpl_motd(sock, "						.@@@@@@@@@@@@@@@@@@@@@@@@     @@@@@@@@@@@@@@           *@@@@@@@@@@  @  @@@@     @@@@@@@@@@           %@@@@@@@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@@@@@");
	rpl_motd(sock, "					@@@@@@@@@@@@@@@@@@@@@@@      @@@@@@@@@@ @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  @@@@@@  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ @@@@@@@@@&      @@@@@@@@@@@@@@@@@@@@@@@");
	rpl_motd(sock, "					   @@@@@@@@@@@@@&       @@@@@@@@@@@@ /@@@@@@@@@@@@@@@@@@@@@@@@@@ &@@@@@  @@@@@@@@  @@@@@ @@@@@@@@@@@@@@@@@@@@@@@@@@@  @@@@@@@@@@@        @@@@@@@@@@@@@@");
	rpl_motd(sock, "						  @@@@          @@@@@@@@@@@@   @@@@@@@@ @@@@@@@@@@@@@@@@@@@  @@@@@  @@@@@@@@@@  @@@@@ @@@@@@@@@@@@@@@@@@@@ @@@@@@@   @@@@@@@@@@@@@         @@@@@");
	rpl_motd(sock, "								   @@@@@@@@@@@@@@    @@@@@@@@ @@@@@@  @@@@ @@@@  @ @@@@@@  @@@@@@@@@@@@  @@@@@@ @ @@@@@@@@@@ @@@@@@  @@@@@@@@   @@@@@@@@@@@@@@");
	rpl_motd(sock, "								@@@@@@@@@@@@@@    @@@@@@@@@  @@@@@@  @@@@@ @@@@  @@@@@@@  @@@@@@@@@@@@@@  @@@@@@  @@@@@ @@@@@ @@@@@@@  @@@@@@@@    @@@@@@@@@@@@@@@");
	rpl_motd(sock, "								   @@@@@@@@     @@@@@@@@@  @@@@@@@  @@@@@  @@@  @@@@@@@  @@@@@@@@@@@@@@@.  @@@@@@@  @@@ @@@@@@  @@@@@@   @@@@@@@@@    @@@@@@@@@");
	rpl_motd(sock, "									  @@      @@@@@@@@@   @@@@@@@  @@@@@@  @  @@@@@@@@  @@@@@@@@@@@@@@@@@   @@@@@@@@  @  @@@@@   @@@@@@@  @@@@@@@@@@      @@");
	rpl_motd(sock, "										   @@@@@@@@@@   @@@@@@@@  @@@@@@         @@@@  @@@@@@@@@@@@@@@@@@@   @@@@         @@@@@   @@@@@@@   @@@@@@@@@@");
	rpl_motd(sock, "											 @@@@@@    @@@@@@@@   @@@@                @@@@@@@@@@@@@@@@@@@@@                @@@@@   @@@@@@@@   @@@@@@");
	rpl_motd(sock, "													  @@@@@@@@   @@                  @@@@@@@@@@@@@@@@@@@@@@@                   @@   @@@@@@@@");
	rpl_motd(sock, "																					   @@@@@@@@@@@@@@@@@@@");
	rpl_motd(sock, "																				    @@@   @@@@@@@@@@@@@    @@@");
	rpl_motd(sock, "																				  @@@@@   @@@@@@@@@@@@@@  @@@@@");
	rpl_motd(sock, "																				 @@@@@   @@, @@@@@@@@ @@@  @@@@@ ");
	rpl_motd(sock, "																		 @@@@@@@@@@@@   @@  @@*@@@ @@@ @@@   @@@@@@@@@@@");
	rpl_motd(sock, "																		  @@@@@@@@@@@  @@  @@@ @@@@ @@@ @@   @@@@@@@@@@@");
	rpl_motd(sock, "																		  @  @@@@ @@@@     @@ @@@@@@ @@    @@@@@*@@@");
	rpl_motd(sock, "																			@@@   @@@        @@@@@@@         @@@  @@@@");
	rpl_motd(sock, "																		   @@@                 @@@@                 @@@");
	rpl_motd(sock, "																		   @@@@                                    @@@@");

	rpl_endofmotd(sock);
}

eResult Irisha::MOTD(const int sock)
{
	User*	user;
	if (check_user(sock, user, cmd_.prefix_) == R_FAILURE)
		return R_FAILURE;

	if (cmd_.arguments_.empty() || cmd_.arguments_[0] == domain_)
		send_motd(sock);
	else
	{
		Server*	server = find_server(cmd_.arguments_[0]);
		if (server == nullptr)
		{
			err_nosuchserver(sock, cmd_.arguments_[0]);
			return R_FAILURE;
		}
		send_msg(choose_sock(server), user->nick(), "MOTD :" + cmd_.arguments_[0]);
	}
	return R_SUCCESS;
}

/**
 * @description	Handles MOTD reply
 * @param		sock
 * @return
 */
eResult Irisha::MOTD_REPLIES(const int sock)
{
	if (!is_enough_args(sock, cmd_.command_, 2))
		return R_FAILURE;

	User*	user = find_user(cmd_.arguments_[0]);
	if (user == nullptr)
	{
		err_nosuchnick(sock, cmd_.arguments_[0]);
		return R_FAILURE;
	}
	send_msg(choose_sock(user), domain_,cmd_.command_
					+ " " + cmd_.arguments_[0] + " " + cmd_.arguments_[1]);
	return R_SUCCESS;
}

void Irisha::count_operators(int& operators) const
{
	operators = 0;
	User* user;
	for (con_const_it it = connections_.begin(); it != connections_.end(); ++it)
	{
		if (it->second->type() == T_CLIENT)
		{
			user = static_cast<User*>(it->second);
			if (user->is_operator())
				++operators;
		}
	}
}

void Irisha::count_global(int& users, int& servers) const
{
	users = 0;
	servers = 0;
	for (con_const_it it = connections_.begin(); it != connections_.end(); ++it)
	{
		if (it->second->type() == T_CLIENT)
			++users;
		else if (it->second->type() == T_SERVER)
			++servers;
	}
}

void Irisha::count_local(int& users, int& servers) const
{
	users = 0;
	servers = 0;
	for (con_const_it it = connections_.begin(); it != connections_.end(); ++it)
	{
		if (it->second->socket() != U_EXTERNAL_CONNECTION)
		{
			if (it->second->type() == T_CLIENT)
				++users;
			else if (it->second->type() == T_SERVER)
				++servers;
		}
	}
}

void Irisha::send_lusers_replies(const int sock) const
{
	rpl_luserclient(sock);
	rpl_luserop(sock);
	rpl_luserunknown(sock);
	rpl_luserchannels(sock);
	rpl_luserme(sock);
}

/**
 * @description	Handles LUSERS command
 * @param		sock
 * @return
 */
eResult Irisha::LUSERS(const int sock) //! TODO: handle replies to and from other servers
{
	User*	user;
	if (check_user(sock, user, cmd_.prefix_) == R_FAILURE)
		return R_FAILURE;

	if (cmd_.arguments_.empty() || cmd_.arguments_[0] == domain_)
		send_lusers_replies(sock);
	else
	{
		Server*	server = find_server(cmd_.arguments_[0]);
		if (check_server(sock, server, cmd_.arguments_[0]) == R_FAILURE)
			return R_FAILURE;
		send_msg(choose_sock(server), user->nick(), "MOTD :" + cmd_.arguments_[0]);
	}
	return R_SUCCESS;
}

/**
 * @description	Handles LUSERS replies
 * @param		sock
 * @return
 */
eResult Irisha::LUSERS_REPLIES(const int sock)
{
	if (!is_enough_args(sock, cmd_.command_, 2))
		return R_FAILURE;

	User*	user = find_user(cmd_.arguments_[0]);
	if (user == nullptr)
	{
		err_nosuchnick(sock, cmd_.arguments_[0]);
		return R_FAILURE;
	}
	send_msg(choose_sock(user), domain_,cmd_.command_
										+ " " + cmd_.arguments_[0] + " " + cmd_.arguments_[1]);
	return R_SUCCESS;
}

/**
 * @description	Handles SQUIT command (disconnects servers)
 * @param		sock
 * @return		R_SUCCESS or R_FAILURE
 */
eResult Irisha::SQUIT(const int sock)
{
	if (!is_enough_args(sock, cmd_.command_, 2))
		return R_FAILURE;

	Server*	server = find_server(cmd_.arguments_[0]);
	if (cmd_.arguments_[0] == domain_)
	{
		send_servers(domain_, "SQUIT " + domain_ + " :received SQUIT command", sock);
		sys_msg(E_SLEEP, "Shutting down.");
		exit(0);
	}
	else if (check_server(sock, server, cmd_.arguments_[0]) == R_FAILURE)
		return R_FAILURE;

	if (cmd_.type_ == T_SERVER)
		send_servers(cmd_.line_, sock);
	else if (cmd_.prefix_ == "")
	{
		if (!is_user_operator(sock))
			return R_FAILURE;
		send_msg(choose_sock(server), connection_name(sock), "SQUIT "
			+ cmd_.arguments_[0] + " :" + cmd_.arguments_[1]);
	}
	else
		send_msg(choose_sock(server), cmd_.line_);
	sys_msg(E_BOOM, "Server", server->name(), "disconnected!");
	if (server->socket() != U_EXTERNAL_CONNECTION)
		remove_local_server(server);
	else
		remove_server(server);

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