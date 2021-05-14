//
// Created by Parfait Kentaurus on 5/14/21.
//

#include "Irisha.hpp"
#include "User.hpp"
#include "Server.hpp"
#include "utils.hpp"

/**
 * @description	handles NICK command
 * @param cmd: Command structure
 * @param socket: command sender
 */
void Irisha::NICK(const Command& cmd, const int sock) //! TODO: handle hopcount
{
	if (cmd.arguments.empty()) // NICK command without params
	{
		send_msg(sock, domain_, ":No nickname given"); //! TODO: change to error reply
		return;
	}
	std::string new_nick = cmd.arguments[0];
	std::map<int, AConnection*>::iterator it = connections_.find(sock);

	if (it == connections_.end())	// Add new user
	{
		if (!is_a_valid_nick(new_nick))
			send_msg(sock, domain_, new_nick + " :Erroneus nickname"); //! TODO: change to error reply
		add_user(sock, new_nick);
	}
	else							// Change nickname
	{
		User* user = dynamic_cast<User *>(it->second);
		if (user == nullptr) // if user is not local
		{
			find_user(cmd.prefix)->set_nick(new_nick);
			// TODO: send message to next server
		}
		else
		{
			if (find_user(new_nick))
			{
				send_msg(sock, domain_, new_nick + " :Nickname is already in use"); //! TODO: change to error reply
				return;
			}
			user->set_nick(new_nick);
		}
	}
}

/**
 * Handling PASS message
 * @param fd - socket
 * @param cmd - parsed message
 * @return 0 if password correct, else 1
 */
int Irisha::PASS(int fd)
{
	if (cmd_.arguments.empty())
		return 1;
	if (password_ == cmd_.arguments[0])
		return 0;
	else
		return 1;
}

/**
 *Handling SERVER message
 * @param fd - socket
 * @param cmd - parsed message
 * @return 0 if registration successfully, else 1
 */
int Irisha::SERVER(int fd)
{
	//place for your validator
	int hopcount = 1;

	if (cmd_.prefix.empty())
		hopcount = 1;
	//else
	//take hopcount from argiments
	AConnection* server = new Server(cmd_.arguments[0], fd, hopcount);
	connections_.insert(std::pair<int, AConnection*>(fd, server));
	std::cout << PURPLE "Server " << (static_cast<Server*>(server))->name() << " registered!" CLR << std::endl;
	return 0;
}