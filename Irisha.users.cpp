
#include "Irisha.hpp"
#include "User.hpp"
#include "utils.hpp"

/**
 * @description	Adds local user
 * @param		sock: user socket
 * @param		nick: user nick
 */
void Irisha::add_user(const int sock, const std::string& nick)
{
	User* user = new User(sock, domain_, nick);
	connections_.insert(std::pair<std::string, AConnection*>(nick, user));
}

/**
 * @description	Adds user by other server request
 * @param		sock: user socket
 */
void Irisha::add_user(int source_sock) // [0]<nick> [1]<hopcount> [2]<username> [3]<host> [4]<servertoken> [5]<umode> [6]<realname>
{
	User* user = new User(U_EXTERNAL_CONNECTION, cmd_.arguments_[3],
					   str_to_int(cmd_.arguments_[1]), source_sock,
					   str_to_int(cmd_.arguments_[4]));

	user->set_nick(cmd_.arguments_[0]);
	user->set_username(cmd_.arguments_[2]);
//	user->set_mode(cmd_.arguments_[2]);	//! TODO: make mode parsing function
	user->set_realname(cmd_.arguments_[2]);
	connections_.insert(std::pair<std::string, AConnection*>(cmd_.arguments_[0], user));

	sys_msg(E_ALIEN, "New external user", cmd_.arguments_[0], "registered!");
}

/**
 * @description	Removes user by nick
 * @param		nick
 */
void Irisha::remove_user(const std::string& nick)
{
	User* user = find_user(nick);
	if (user == nullptr)
	{
		std::cout << E_CROSS RED "Can't remove user " + nick + CLR << std::endl;
		return;
	}
	connections_.erase(nick);
	delete user;
}

/**
 * @description	Finds user by nick
 * @param		nick
 * @return		user pointer or nullptr
 */
User* Irisha::find_user(const std::string& nick) const
{
	User*			user;
	con_const_it	it = connections_.begin();
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

/**
 * @description	Finds user by socket
 * @param		sock: socket
 * @return		user pointer or nullptr
 */
User* Irisha::find_user(const int sock) const
{
	User*			user;
	con_const_it	it = connections_.begin();
	for (; it != connections_.end(); ++it)
	{
		if (it->second->type() == T_CLIENT)
		{
			user = static_cast<User*>(it->second);
			if (user->socket() == sock)
				return user;
		}
	}
	return nullptr;
}
