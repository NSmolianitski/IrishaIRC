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
void Irisha::err_nosuchserver(const int sock, const std::string& server_name)
{
	std::string	receiver = connection_name(sock);
	send_rpl_msg(sock, ERR_NOSUCHSERVER, receiver, server_name + " " + ":No such server");
}

/// Common replies
void Irisha::rpl_time(const int sock, const std::string& server, const std::string& local_time)
{
	std::string	receiver = connection_name(sock);
	send_rpl_msg(sock, RPL_TIME, receiver, server + " " + local_time);
}