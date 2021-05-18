//
// Created by Parfait Kentaurus on 5/14/21.
//

#include <sstream>
#include "Irisha.hpp"
#include "User.hpp"
#include "Server.hpp"
#include "utils.hpp"

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
		send_msg(sock, domain_, new_nick + " :Nickname is already in use"); //! TODO: change to error reply
		return R_FAILURE;
	}
	connection->set_nick(new_nick);
	send_msg(sock, old_nick, "NICK " + new_nick); // Reply for user TODO: check prefix
	// TODO: send message to next server

	std::cout << ITALIC PURPLE E_GEAR " User " BWHITE + old_nick + PURPLE " changed nick to " BWHITE
				 + new_nick << " " CLR << std::endl;
	return R_SUCCESS;
}

/**
 * @description	Handles request from other server (changes nick or adds new user)
 * @param		new_nick: new nickname
 * @return		R_SUCCESS
 */
eResult	Irisha::NICK_server(const std::string& new_nick)
{
	User*		user;
	std::string	old_nick;

	user = find_user(cmd_.prefix_);
	if (user == nullptr)		// Add new external user
	{
		add_user();
		return R_SUCCESS;
	}
	old_nick = user->nick();
	user->set_nick(new_nick);	// Change nick for external user
	// TODO: send message to next server

	std::cout << ITALIC PURPLE E_GEAR "User " BWHITE + old_nick + PURPLE " changed nick to " BWHITE
				 + new_nick << " " CLR << std::endl;
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
		send_msg(sock, domain_, "431 :No nickname given"); //! TODO: change to error reply
		return R_FAILURE;
	}
	std::string new_nick = cmd_.arguments_[0];
	if (!is_a_valid_nick(new_nick))
	{
		send_msg(sock, domain_, "432 :" + new_nick + " :Erroneous nickname"); //! TODO: change to error reply
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
			return NICK_server(new_nick);
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
		send_msg(sock, domain_, "461 :USER :Not enough parameters"); //! TODO: change to error reply
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
		send_msg(sock, domain_, "462 :Unauthorized command (already registered)"); //! TODO: change to error reply
		return R_FAILURE;
	}
	user->set_username(cmd_.arguments_[0]);

	if (cmd_.arguments_[1].length() > 1) //! TODO: don't forget to handle modes
		user->set_mode(0);
	else
		user->set_mode(str_to_int(cmd_.arguments_[1]));
	send_msg(sock, domain_, "001 " + user->nick() + " " + welcome_); //! TODO: change to RPL_WELCOME

	// NICK <nickname> <hopcount> <username> <host> <servertoken> <umode> <realname>
	send_servers(domain_, "NICK " + user->nick() + " 1 " + user->username() + " " + user->server() + " 2 + " + user->username()); //! TODO: add server token, user modes, fix realname
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
eResult Irisha::SERVER(const int sock)
{
	//validation+
	std::string info = "";
	for (int i = 0; i < cmd_.arguments_.size(); i++)
	{
		if (cmd_.arguments_[i].find(':') != std::string::npos)
		{
			info = cmd_.arguments_[i];
			break ;
		}
	}
	if (info.empty())
		return R_FAILURE;
	//validation-

	int hopcount;
	if (cmd_.arguments_.empty())
		return R_FAILURE;
	if (cmd_.arguments_.size() == 1) //only server name sent
		hopcount = 1;
	else if (cmd_.arguments_.size() == 2 &&
		(cmd_.arguments_[0].find_first_not_of("0123456789") == std::string::npos)) //hopcount and server name sent
	{
		try	{ hopcount = std::stoi(cmd_.arguments_[0]); }
		catch(std::exception ex) { return R_FAILURE; }
	}
	else
		hopcount = str_to_int(cmd_.arguments_[0]); ///TODO: remove it

	AConnection* server = new Server(cmd_.arguments_[0], sock, hopcount);
	connections_.insert(std::pair<std::string, AConnection*>(cmd_.arguments_[0], server));
	PING(sock);
	sys_msg(E_LAPTOP, "Server", cmd_.arguments_[0], "registered!");
	return R_SUCCESS;
}

eResult Irisha::PONG(const int sock)
{
	if (cmd_.arguments_.empty())
		return R_FAILURE; ///TODO: send ERR_NOORIGIN
	else if (cmd_.arguments_.size() == 1)
		send_msg(sock, domain_, "PONG " + domain_);
	//else send to receiver
	return R_SUCCESS;
}

eResult Irisha::PING(const int sock)
{
	if (cmd_.arguments_.empty())
		return R_FAILURE; ///TODO: send ERR_NOORIGIN
	else if (cmd_.arguments_.size() == 1) // PINGing this server
		PONG(sock);
	//else send to receiver
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