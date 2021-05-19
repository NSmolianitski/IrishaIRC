//
// Created by Parfait Kentaurus on 5/19/21.
//

#include "Irisha.hpp"

/**
 * @description	Sends reply message
 * @param		sock
 * @param		rpl: reply code
 * @param		target: receiver
 * @param		msg: reply message
 */
void			Irisha::send_rpl_msg		(int sock, eReply rpl, const std::string& target, const std::string& msg) const
{
	std::string message = rpl_code_to_str(rpl) + " " + target + " " + msg;
	send_msg(sock, domain_, message);
}

/**
 * @description	Sends error reply message
 * @param		sock
 * @param		rpl: error reply code
 * @param		target: receiver
 * @param		msg: reply message
 */
void			Irisha::send_rpl_msg		(int sock, eError rpl, const std::string& target, const std::string& msg) const
{
	std::string message = rpl_code_to_str(rpl) + " " + target + " " + msg;
	send_msg(sock, domain_, message);
}

/// Error replies
void Irisha::err_nosuchserver(const int sock, const std::string& server_name) const
{
	std::string	receiver = connection_name(sock);
	send_rpl_msg(sock, ERR_NOSUCHSERVER, receiver, server_name + " :No such server");
}

void Irisha::err_nonicknamegiven(const int sock) const
{
	std::string	receiver = connection_name(sock);
	send_rpl_msg(sock, ERR_NONICKNAMEGIVEN, receiver, ":No nickname given");
}

void Irisha::err_nosuchnick(const int sock, const std::string& nick) const
{
	std::string	receiver = connection_name(sock);
	send_rpl_msg(sock, ERR_NOSUCHNICK, receiver, nick + " :No such nick/channel");
}

void Irisha::err_erroneusnickname(const int sock, const std::string& nick) const
{
	std::string	receiver = connection_name(sock);
	send_rpl_msg(sock, ERR_ERRONEUSNICKNAME, receiver, nick + " :Erroneus nickname");
}

void Irisha::err_nicknameinuse(const int sock, const std::string& nick) const
{
	std::string	receiver = connection_name(sock);
	send_rpl_msg(sock, ERR_NICKNAMEINUSE, receiver, nick + " :Nickname is already in use");
}

void Irisha::err_nickcollision(const int sock, const std::string& nick) const
{
	std::string	receiver = connection_name(sock);
	send_rpl_msg(sock, ERR_NICKCOLLISION, receiver, nick + " :Nickname collision KILL");
}

void Irisha::err_nosuchchannel(const int sock, const std::string& channel_name) const
{
	std::string	receiver = connection_name(sock);
	send_rpl_msg(sock, ERR_NOSUCHCHANNEL, receiver, channel_name + " :No such channel");
}

void Irisha::err_needmoreparams(const int sock, const std::string& command) const
{
	std::string	receiver = connection_name(sock);
	send_rpl_msg(sock, ERR_NEEDMOREPARAMS, receiver, command + " :Not enough parameters");
}

void Irisha::err_alreadyregistered(const int sock) const
{
	std::string	receiver = connection_name(sock);
	send_rpl_msg(sock, ERR_ALREADYREGISTRED, receiver, ":You may not reregister");
}

/// Common replies
void Irisha::rpl_welcome(const int sock) const
{
	std::string	receiver = connection_name(sock);
	send_rpl_msg(sock, RPL_WELCOME, receiver, welcome_);
}

void Irisha::rpl_youreoper(const int sock) const
{
	std::string	receiver = connection_name(sock);
	send_rpl_msg(sock, RPL_YOUREOPER, receiver, ":You are now an IRC operator");
}

void Irisha::rpl_time(const int sock, const std::string& server, const std::string& local_time) const
{
	std::string	receiver = connection_name(sock);
	send_rpl_msg(sock, RPL_TIME, receiver, server + " " + local_time);
}
