//
// Created by Parfait Kentaurus on 5/14/21.
//

#include "Irisha.hpp"
#include "User.hpp"
#include "utils.hpp"

/**
 * @description	Adds user
 * @param		sock: user socket
 * @param		nick: user nick
 */
void Irisha::add_user(int sock, const std::string& nick)
{
	AConnection* user = new User(sock, domain_, nick);
	connections_.insert(std::pair<int, AConnection*>(sock, user));
}

/**
 * @description	Removes user by nick
 * @param		nick
 */
void Irisha::remove_user(const std::string& nick)
{
	AConnection* user = find_user(nick);
	connections_.erase(user->socket());
}

/**
 * @description	Finds user by nick
 * @param		nick
 * @return		user pointer
 */
User* Irisha::find_user(const std::string& nick) const
{
	User*	user;
	std::map<int, AConnection*>::const_iterator it = connections_.begin();
	for (; it != connections_.end(); ++it)
	{
		if (it->second->type() == T_CLIENT)
		{
			user = static_cast<User*>(it->second);
			if (user->nick() == nick)
				return user;
		}
	}
	return nullptr;
}

void Irisha::print_user_list()
{
	User*	user;
	std::map<int, AConnection*>::const_iterator it = connections_.begin();
	for (; it != connections_.end(); ++it)
	{
		if (it->second->type() == T_CLIENT)
		{
			user = static_cast<User*>(it->second);
			std::cout << "[" PURPLE "CONNECTED USERS" CLR "] " BWHITE ITALIC << user->nick() << " " CLR << std::endl;
		}
	}
}