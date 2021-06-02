
#include "Irisha.hpp"

/**
 * @description	Sends reply message
 * @param		sock
 * @param		rpl: reply code
 * @param		msg: reply message
 */
void			Irisha::send_rpl_msg		(int sock, eReply rpl, const std::string& msg) const
{
	std::string	target = connection_name(sock);
	std::string message = rpl_code_to_str(rpl) + " " + target + " " + msg;
	send_msg(sock, domain_, message);
}

/**
 * @description	Sends reply message with receiver name
 * @param		sock
 * @param		rpl: reply code
 * @param		msg: reply message
 * @param		receiver
 */
void			Irisha::send_rpl_msg		(int sock, eReply rpl, const std::string& msg
							  					, const std::string& target) const
{
	std::string message = rpl_code_to_str(rpl) + " " + target + " " + msg;
	send_msg(sock, domain_, message);
}

/**
 * @description	Sends error reply message
 * @param		sock
 * @param		rpl: error reply code
 * @param		msg: reply message
 */
void			Irisha::send_rpl_msg		(int sock, eError rpl, const std::string& msg) const
{
	std::string	target = connection_name(sock);
	std::string message = rpl_code_to_str(rpl) + " " + target + " " + msg;
	send_msg(sock, domain_, message);
}

/**
 * @description	Sends error reply message with receiver name
 * @param		sock
 * @param		rpl: error reply code
 * @param		msg: reply message
 * @param		receiver
 */
void			Irisha::send_rpl_msg		(int sock, eError rpl, const std::string& msg
							  					, const std::string& target) const
{
	std::string message = rpl_code_to_str(rpl) + " " + target + " " + msg;
	send_msg(sock, domain_, message);
}

/// Error replies
void Irisha::err_nosuchserver(const int sock, const std::string& server) const
{
	send_rpl_msg(sock, ERR_NOSUCHSERVER, server + " :No such server");
}

void Irisha::err_nonicknamegiven(const int sock) const
{
	send_rpl_msg(sock, ERR_NONICKNAMEGIVEN, ":No nickname given");
}

void Irisha::err_nosuchnick(const int sock, const std::string& nick) const
{
	send_rpl_msg(sock, ERR_NOSUCHNICK, nick + " :No such nick/channel");
}

void Irisha::err_erroneusnickname(const int sock, const std::string& nick) const
{
	send_rpl_msg(sock, ERR_ERRONEUSNICKNAME, nick + " :Erroneus nickname");
}

void Irisha::err_nicknameinuse(const int sock, const std::string& nick) const
{
	send_rpl_msg(sock, ERR_NICKNAMEINUSE, nick + " :Nickname is already in use");
}

void Irisha::err_nickcollision(const int sock, const std::string& nick) const
{
	send_rpl_msg(sock, ERR_NICKCOLLISION, nick + " :Nickname collision KILL");
}

void Irisha::err_nosuchchannel(const int sock, const std::string& channel) const
{
	send_rpl_msg(sock, ERR_NOSUCHCHANNEL, channel + " :No such channel");
}

void Irisha::err_needmoreparams(const int sock, const std::string& command) const
{
	send_rpl_msg(sock, ERR_NEEDMOREPARAMS, command + " :Not enough parameters");
}

void Irisha::err_alreadyregistered(const int sock) const
{
	send_rpl_msg(sock, ERR_ALREADYREGISTRED, ":You may not reregister");
}

void Irisha::err_noorigin(const int sock) const
{
	send_rpl_msg(sock, ERR_NOORIGIN, ":No origin specified");
}

void Irisha::err_norecipient(const int sock, const std::string& command) const
{
	send_rpl_msg(sock, ERR_NORECIPIENT, ":No recipient given (" + command + ")");
}

void Irisha::err_notexttosend(const int sock) const
{
	send_rpl_msg(sock, ERR_NOTEXTTOSEND, ":No text to send");
}

void Irisha::err_notoplevel(const int sock, const std::string& mask) const
{
	send_rpl_msg(sock, ERR_NOTOPLEVEL, ":No toplevel domain specified");
}

void Irisha::err_wildtoplevel(const int sock, const std::string& mask) const
{
	send_rpl_msg(sock, ERR_WILDTOPLEVEL, mask + " :Wildcard in toplevel domain");
}

void Irisha::err_cannotsendtochan(const int sock, const std::string& channel) const
{
	send_rpl_msg(sock, ERR_CANNOTSENDTOCHAN, channel + " :Cannot send to channel");
}

void Irisha::err_toomanytargets(const int sock, const std::string& target) const
{
	send_rpl_msg(sock, ERR_TOOMANYTARGETS, target + " :Duplicate recipients. No message delivered");
}

void Irisha::err_unknowncommand(const int sock, const std::string& command) const
{
	send_rpl_msg(sock, ERR_UNKNOWNCOMMAND, command + " :Unknown command");
}

void Irisha::err_chanoprivsneeded(const int sock, const std::string& channel) const
{
	send_rpl_msg(sock, ERR_CHANOPRIVSNEEDED, channel + " :You're not channel operator");
}

void Irisha::err_notochannel(const int sock, const std::string& channel) const
{
	send_rpl_msg(sock, ERR_NOTONCHANNEL, channel + " :You're not on that channel");
}

void Irisha::err_keyset(const int sock, const std::string& channel) const
{
	send_rpl_msg(sock, ERR_KEYSET, channel + " :Channel key already set");
}

void Irisha::err_unknownmode(const int sock, const std::string& mode_char) const
{
	send_rpl_msg(sock, ERR_UNKNOWNMODE, mode_char + " :is unknown mode char to me");
}

void Irisha::err_usersdontmatch(const int sock) const
{
	send_rpl_msg(sock, ERR_USERSDONTMATCH, ":Cannot change mode for other users");
}

void Irisha::err_umodeunknownflag(const int sock) const
{
	send_rpl_msg(sock, ERR_UMODEUNKNOWNFLAG, ":Unknown MODE flag");
}

void Irisha::err_bannedfromchan(const int sock, const std::string& channel) const
{
	send_rpl_msg(sock, ERR_BANNEDFROMCHAN, channel + " :Cannot join channel (+b)");
}

void Irisha::err_initeonlychan(const int sock, const std::string& channel) const
{
	send_rpl_msg(sock, ERR_INVITEONLYCHAN, channel + " :Cannot join channel (+i)");
}

void Irisha::err_channelisfull(const int sock, const std::string& channel) const
{
	send_rpl_msg(sock, ERR_CHANNELISFULL, channel + " :Cannot join channel (+l)");
}

void Irisha::err_toomanychannels(const int sock, const std::string& channel) const
{
	send_rpl_msg(sock, ERR_TOOMANYCHANNELS, channel + " :You have joined too many channels");
}

void Irisha::err_noprivileges(const int sock) const
{
	send_rpl_msg(sock, ERR_NOPRIVILEGES, ":Permission Denied- You're not an IRC operator");
}

void Irisha::err_useronchannel(const int sock, const std::string& user, const std::string& channel) const
{
	send_rpl_msg(sock, ERR_USERONCHANNEL, user + channel +" :is already on channel");
}

void Irisha::err_usersdisabled(const int sock) const
{
	send_rpl_msg(sock, ERR_USERSDISABLED, ":USERS has been disabled");
}

void Irisha::err_notregistered(const int sock) const
{
	send_rpl_msg(sock, ERR_NOTREGISTERED, ":You have not registered");
}

void Irisha::err_yourebannedcreep(const int sock) const
{
	send_rpl_msg(sock, ERR_YOUREBANNEDCREEP, ":You are banned from this server");
}

void Irisha::err_nooperhost(const int sock) const
{
	send_rpl_msg(sock, ERR_NOOPERHOST, ":No O-lines for your host");
}

void Irisha::err_passwdmismatch(const int sock) const
{
	send_rpl_msg(sock, ERR_PASSWDMISMATCH, ":Password incorrect");
}

void Irisha::err_cantkillserver(const int sock) const
{
	send_rpl_msg(sock, ERR_CANTKILLSERVER, ":You cant kill a server!");
}

/// Common replies
void Irisha::rpl_welcome(const int sock) const
{
	send_rpl_msg(sock, RPL_WELCOME, welcome_);
}

void Irisha::rpl_youreoper(const int sock) const
{
	send_rpl_msg(sock, RPL_YOUREOPER, ":You are now an IRC operator");
}

void Irisha::rpl_time(const int sock, const std::string& server, const std::string& local_time) const
{
	send_rpl_msg(sock, RPL_TIME, server + " " + local_time);
}

void Irisha::rpl_away(const int sock, const std::string& nick, const std::string& away_msg) const
{
	send_rpl_msg(sock, RPL_AWAY, nick + " " + away_msg);
}

void Irisha::rpl_channelmodeis(const int sock, const std::string& mode, const std::string& mode_params) const
{
	send_rpl_msg(sock, RPL_CHANNELMODEIS, mode + " " + mode_params);
}

void Irisha::rpl_banlist(const int sock, const std::string& channel, const std::string& ban_id) const
{
	send_rpl_msg(sock, RPL_BANLIST, channel + " " + ban_id);
}

void Irisha::rpl_endofbanlist(const int sock, const std::string& channel) const
{
	send_rpl_msg(sock, RPL_ENDOFBANLIST, channel + " :End of channel ban list");
}

void Irisha::rpl_info(const int sock, const std::string& info) const
{
	send_rpl_msg(sock, RPL_INFO, ":" + info);
}

void Irisha::rpl_endofinfo(const int sock) const
{
	send_rpl_msg(sock, RPL_ENDOFINFO, ":End of /INFO list");
}

void Irisha::rpl_motdstart(const int sock, const std::string& server) const
{
	send_rpl_msg(sock, RPL_MOTDSTART, ":- " + server + " Message of the day - ");
}

void Irisha::rpl_motd(const int sock, const std::string& text) const
{
	send_rpl_msg(sock, RPL_MOTD, ":- " + text);
}

void Irisha::rpl_endofmotd(const int sock) const
{
	send_rpl_msg(sock, RPL_ENDOFMOTD, ":End of /MOTD command");
}

void Irisha::rpl_umodeis(const int sock, const std::string& mode_string) const
{
	send_rpl_msg(sock, RPL_UMODEIS, mode_string);
}

void Irisha::rpl_topic(const int sock, const std::string& channel, const std::string& topic) const
{
	send_rpl_msg(sock, RPL_TOPIC, channel + " :" + topic);
}

void Irisha::rpl_notopic(const int sock, const std::string& channel) const
{
	send_rpl_msg(sock, RPL_NOTOPIC, channel + " :No topic is set");
}

void Irisha::rpl_inviting(const int sock, const std::string& channel, const std::string& nick) const
{
	send_rpl_msg(sock, RPL_INVITING, nick + " " + channel);
}

void Irisha::rpl_version(const int sock, const std::string& target, const std::string& version, const std::string& debug_lvl
						 	, const std::string& server, const std::string& comments) const
{
	send_msg(sock, domain_, rpl_code_to_str(RPL_VERSION) + " " + target + " " + version + "." +
		debug_lvl + " " + server + " :" + comments);
}

void Irisha::rpl_adminme(const int sock, const std::string& target, const std::string& server) const
{
	send_rpl_msg(sock, RPL_ADMINME, server + " :Administrative info", target);
}

void Irisha::rpl_adminloc1(const int sock, const std::string& target, const std::string& info) const
{
	send_rpl_msg(sock, RPL_ADMINLOC1, ":" + info, target);
}

void Irisha::rpl_adminloc2(const int sock, const std::string& target, const std::string& info) const
{
	send_rpl_msg(sock, RPL_ADMINLOC2, ":" + info, target);
}

void Irisha::rpl_adminmail(const int sock, const std::string& target, const std::string& info) const
{
	send_rpl_msg(sock, RPL_ADMINEMAIL, ":" + info, target);
}

void Irisha::rpl_luserclient(const int sock) const
{
	int users, servers;

	count_global(users, servers);
	send_rpl_msg(sock, RPL_LUSERCLIENT, ":There are " + int_to_str(users)
						+ " users on " + int_to_str(servers) + " servers"); //! TODO: services?
}

void Irisha::rpl_luserop(const int sock) const
{
	int operators;

	count_operators(operators); //! TODO: check is_operator function
	if (operators != 0)
		send_rpl_msg(sock, RPL_LUSEROP, int_to_str(operators) + " operator(s) online");
}

void Irisha::rpl_luserchannels(const int sock) const
{
	int channels = static_cast<int>(channels_.size());

	if (channels != 0)
		send_rpl_msg(sock, RPL_LUSERCHANNELS, int_to_str(channels) + " :channels formed");
}

void Irisha::rpl_luserunknown(const int sock) const
{
	int unknown_connections = 0; //! TODO: count unknown connections (in registration)

	if (unknown_connections != 0)
		send_rpl_msg(sock, RPL_LUSERUNKNOWN, int_to_str(unknown_connections) + " :unknown connection(s)");
}

void Irisha::rpl_luserme(const int sock) const
{
	int users, servers;

	count_local(users, servers);
	send_rpl_msg(sock, RPL_LUSERCLIENT, ":I have " + int_to_str(users)
										+ " clients and " + int_to_str(servers) + " servers");
}

void Irisha::rpl_endofstats(const int sock, const std::string& letter, const std::string &target) const
{
	send_rpl_msg(sock, RPL_ENDOFSTATS,letter + " :End of /STATS report", target);
}

void Irisha::rpl_statslinkinfo(const int sock, const std::string &msg, const std::string &target)
{
	send_rpl_msg(sock, RPL_STATSLINKINFO, msg, target);
}

void Irisha::rpl_statsuptime(const int sock, const std::string &msg, const std::string &target)
{
	send_rpl_msg(sock, RPL_STATSUPTIME, msg, target);
}

void Irisha::rpl_links(const int sock, const std::string &serv_name, int hopcount, const std::string &target)
{
	send_rpl_msg(sock, RPL_LINKS, serv_name + " :" + int_to_str(hopcount), target);
}

void Irisha::rpl_endoflinks(const int sock, const std::string &serv_name, const std::string &target)
{
	send_rpl_msg(sock, RPL_ENDOFLINK, serv_name + " :End of /LINKS list", target);
}