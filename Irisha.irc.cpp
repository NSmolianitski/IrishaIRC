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
 * @description	handles NICK command
 * @param cmd: Command structure
 * @param socket: command sender
 */
CmdResult	Irisha::NICK(const int sock) //! TODO: handle hopcount
{
	if (cmd_.arguments.empty())	// NICK command without params
	{
		send_msg(sock, domain_, ":No nickname given"); //! TODO: change to error reply
		return CMD_FAILURE;
	}

	std::string new_nick = cmd_.arguments[0];
	std::map<int, AConnection*>::iterator it = connections_.find(sock);
	if (it == connections_.end())	// Add new user
	{
		if (!is_a_valid_nick(new_nick))
			send_msg(sock, domain_, new_nick + " :Erroneous nickname"); //! TODO: change to error reply
		add_user(sock, new_nick);
	}
	else							// Change nickname
	{
		User* user = dynamic_cast<User *>(it->second);
		if (user == nullptr)	// If user is NOT local
		{
			find_user(cmd_.prefix)->set_nick(new_nick);
			// TODO: send message to next server
		}
		else					// If user is local
		{
			if (find_user(new_nick))
			{
				send_msg(sock, domain_, new_nick + " :Nickname is already in use"); //! TODO: change to error reply
				return CMD_FAILURE;
			}
			send_msg(sock, user->nick(), "NICK " + new_nick); //! TODO: check prefix
			user->set_nick(new_nick);
		}
	}
	print_user_list(); //! TODO: remove
	return CMD_SUCCESS;
}

/**
 * Handling PASS message
 * @param fd - socket
 * @param cmd - parsed message
 * @return 0 if password correct, else 1
 */
CmdResult Irisha::PASS(const int sock)
{
	if (cmd_.arguments.empty())
		return CMD_FAILURE;
	else if (password_ == cmd_.arguments[0] || !cmd_.prefix.empty())
		return CMD_SUCCESS;
	else
		return CMD_FAILURE;
}

/**
 *Handling SERVER message
 * @param fd - socket
 * @param cmd - parsed message
 * @return 0 if registration successfully, else 1
 */
CmdResult Irisha::SERVER(const int sock)
{
	//validation+
	std::string info = "";
	for (int i = 0; i < cmd_.arguments.size(); i++)
	{
		if (cmd_.arguments[i].find(":") != std::string::npos)
		{
			info = cmd_.arguments[i];
			break ;
		}
	}
	if (info.empty())
		return CMD_FAILURE;
	//validation-

	int hopcount;
	if (cmd_.arguments.empty())
		return CMD_FAILURE;
	if (cmd_.arguments.size() == 1) //only server name sent
		hopcount = 1;
	else if (cmd_.arguments.size() == 2 &&
		(cmd_.arguments[0].find_first_not_of("0123456789") == std::string::npos)) //hopcount and server name sent
	{
		try	{ hopcount = std::stoi(cmd_.arguments[0]); }
		catch(std::exception ex) { return CMD_FAILURE; }
	}
	else
		hopcount = std::stoi(cmd_.arguments[0]); ///TODO: remove it

	AConnection* server = new Server(cmd_.arguments[0], sock, hopcount);
	connections_.insert(std::pair<int, AConnection*>(sock, server));
	PING(sock);
	std::cout << PURPLE "Server " << (static_cast<Server*>(server))->name() << " registered!" CLR << std::endl;
	return CMD_SUCCESS;
}

CmdResult Irisha::PONG(const int sock)
{
	if (cmd_.arguments.empty())
		return CMD_FAILURE; ///TODO: send ERR_NOORIGIN
	else if (cmd_.arguments.size() == 1)
		send_msg(sock, domain_, "PONG " + domain_);
	//else send to receiver
	return CMD_SUCCESS;
}

CmdResult Irisha::PING(const int sock)
{
	if (cmd_.arguments.empty())
		return CMD_FAILURE; ///TODO: send ERR_NOORIGIN
	else if (cmd_.arguments.size() == 1) // PINGing this server
		PONG(sock);
	//else send to receiver
	return CMD_SUCCESS;
}