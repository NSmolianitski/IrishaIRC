//
// Created by Parfait Kentaurus on 5/14/21.
//

#include "Irisha.hpp"
#include "User.hpp"
#include "Server.hpp"
#include "utils.hpp"

/**
 * @description	Inserts all supported IRC commands to map
 */
void	Irisha::prepare_commands()
{
	commands_.insert(std::pair<std::string, func>("NICK", &Irisha::NICK));
	commands_.insert(std::pair<std::string, func>("PASS", &Irisha::PASS));
	commands_.insert(std::pair<std::string, func>("SERVER", &Irisha::SERVER));
	commands_.insert(std::pair<std::string, func>("PING", &Irisha::PING));
	commands_.insert(std::pair<std::string, func>("PONG", &Irisha::PONG));
}

/**
 * @description	Changes user nick if it's not busy
 * @param		connection: User or Server
 * @param		sock: socket
 * @param		new_nick: desired nickname
 * @return		CMD_SUCCESS or CMD_FAILURE
 */
CmdResult	Irisha::change_nick(User* connection, const int sock, const std::string& new_nick)
{
	User*		user;
	std::string	old_nick;
	if (connection == nullptr)		// If user is NOT local
	{
		user = find_user(cmd_.prefix_);
		old_nick = user->nick();
		user->set_nick(new_nick);
		// TODO: send message to next server
	}
	else						// If user is local
	{
		user = find_user(new_nick);
		old_nick = connection->nick();
		if (old_nick == new_nick)
			return CMD_SUCCESS;
		if (user)
		{
			send_msg(sock, domain_, new_nick + " :Nickname is already in use"); //! TODO: change to error reply
			return CMD_FAILURE;
		}
		send_msg(sock, connection->nick(), "NICK " + new_nick); //! TODO: check prefix
		connection->set_nick(new_nick);
	}
	std::cout << ITALIC PURPLE "User " BWHITE + old_nick + PURPLE " changed nick to " BWHITE
				 + new_nick << " " E_GEAR CLR << std::endl;
	return CMD_SUCCESS;
}

/**
 * @description	Handles NICK command
 * @param		sock: command sender socket
 */
CmdResult	Irisha::NICK(const int sock) //! TODO: handle hopcount
{
	if (cmd_.arguments_.empty())	// NICK command without params
	{
		send_msg(sock, domain_, ":No nickname given"); //! TODO: change to error reply
		return CMD_FAILURE;
	}
	std::string new_nick = cmd_.arguments_[0];
	if (!is_a_valid_nick(new_nick))
	{
		send_msg(sock, domain_, "432 :" + new_nick + " :Erroneous nickname"); //! TODO: change to error reply
		return CMD_FAILURE;
	}

	con_it it = connections_.find(sock);
	if (it == connections_.end())	// Add new user
		add_user(sock, new_nick);
	else							// Change nickname
	{
		User*	connection = dynamic_cast<User *>(it->second);
		return change_nick(connection, sock, new_nick);
	}
	print_user_list(); //! TODO: remove
	return CMD_SUCCESS;
}

/**
 * @description	Handling PASS message
 * @param		sock: command sender socket
 * @param		cmd: parsed message
 * @return		CMD_SUCCESS if password correct, else CMD_FAILURE
 */
CmdResult Irisha::PASS(const int sock)
{
	if (cmd_.arguments_.empty())
		return CMD_FAILURE;
	else if (password_ == cmd_.arguments_[0] || !cmd_.prefix_.empty())
		return CMD_SUCCESS;
	else
		return CMD_FAILURE;
}

/**
 * @description	Handling SERVER message
 * @param		sock: command sender socket
 * @param		cmd: parsed message
 * @return		CMD_FAILURE if registration successfully, else CMD_FAILURE
 */
CmdResult Irisha::SERVER(const int sock)
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
		return CMD_FAILURE;
	//validation-

	int hopcount;
	if (cmd_.arguments_.empty())
		return CMD_FAILURE;
	if (cmd_.arguments_.size() == 1) //only server name sent
		hopcount = 1;
	else if (cmd_.arguments_.size() == 2 &&
		(cmd_.arguments_[0].find_first_not_of("0123456789") == std::string::npos)) //hopcount and server name sent
	{
		try	{ hopcount = std::stoi(cmd_.arguments_[0]); }
		catch(std::exception ex) { return CMD_FAILURE; }
	}
	else
		hopcount = std::stoi(cmd_.arguments_[0]); ///TODO: remove it

	AConnection* server = new Server(cmd_.arguments_[0], sock, hopcount);
	connections_.insert(std::pair<int, AConnection*>(sock, server));
	PING(sock);
	std::cout << PURPLE "Server " << (static_cast<Server*>(server))->name() << " registered!" CLR << std::endl;
	return CMD_SUCCESS;
}

CmdResult Irisha::PONG(const int sock)
{
	if (cmd_.arguments_.empty())
		return CMD_FAILURE; ///TODO: send ERR_NOORIGIN
	else if (cmd_.arguments_.size() == 1)
		send_msg(sock, domain_, "PONG " + domain_);
	//else send to receiver
	return CMD_SUCCESS;
}

CmdResult Irisha::PING(const int sock)
{
	if (cmd_.arguments_.empty())
		return CMD_FAILURE; ///TODO: send ERR_NOORIGIN
	else if (cmd_.arguments_.size() == 1) // PINGing this server
		PONG(sock);
	//else send to receiver
	return CMD_SUCCESS;
}