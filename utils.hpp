//
// Created by Parfait Kentaurus on 5/9/21.
//

#ifndef FT_IRC_UTILS_HPP
#define FT_IRC_UTILS_HPP

#include <iostream>

///	Font-style and colors
#define RED		"\033[31m"
#define GREEN	"\033[32m"
#define YELLOW	"\033[33m"
#define BLUE	"\033[34m"
#define PURPLE	"\033[35m"
#define CYAN	"\033[36m"
#define WHITE	"\033[37m"

#define BRED	"\033[91m"
#define BGREEN	"\033[92m"
#define BYELLOW	"\033[93m"
#define BBLUE	"\033[94m"
#define BPURPLE	"\033[95m"
#define BCYAN	"\033[96m"
#define BWHITE	"\033[97m"

#define CLR		"\033[0m"
#define BOLD	"\033[1m"
#define ITALIC	"\033[3m"
#define UND		"\033[4m"

/// Emojis
#define E_SPEECH	"\U0001F4AC"
#define E_MAN		"\U0001F468"
#define E_PAGER		"\U0001F4DF"
#define E_GEAR		"⚙"
#define E_PLUG		"🔌"
#define E_CHAIN		"🔗"
#define E_LAPTOP	"💻"
#define E_ALIEN		"👽"
#define E_FIRE		"🔥"
#define E_STAR		"⭐"
#define E_SCULL		"☠"
#define E_BOOM		"🧨"

enum eUtils
{
	U_EXTERNAL_CONNECTION = -1
};

enum eError
{
	ERR_NOSUCHNICK			= 401,
	ERR_NOSUCHSERVER		= 402,
	ERR_NOSUCHCHANNEL		= 403,
	ERR_CANNOTSENDTOCHAN	= 404,
	ERR_TOOMANYCHANNELS		= 405,
	ERR_WASNOSUCHNICK		= 406,
	ERR_TOOMANYTARGETS		= 407,
	ERR_NOORIGIN			= 409,
	ERR_NORECIPIENT			= 411,
	ERR_NOTEXTTOSEND		= 412,
	ERR_NOTOPLEVEL			= 413,
	ERR_WILDTOPLEVEL		= 414,
	ERR_UNKNOWNCOMMAND		= 421,
	ERR_NOMOTD				= 422,
	ERR_NOADMININFO			= 423,
	ERR_FILEERROR			= 424,
	ERR_NONICKNAMEGIVEN		= 431,
	ERR_ERRONEUSNICKNAME	= 432,
	ERR_NICKNAMEINUSE		= 433,
	ERR_NICKCOLLISION		= 436,
	ERR_USERNOTINCHANNEL	= 441,
	ERR_NOTONCHANNEL		= 442,
	ERR_USERONCHANNEL		= 443,
	ERR_NOLOGIN				= 444,
	ERR_SUMMONDISABLED		= 445,
	ERR_USERSDISABLED		= 446,
	ERR_NOTREGISTERED		= 451,
	ERR_NEEDMOREPARAMS		= 461,
	ERR_ALREADYREGISTRED	= 462,
	ERR_NOPERMFORHOST		= 463,
	ERR_PASSWDMISMATCH		= 464,
	ERR_YOUREBANNEDCREEP	= 465,
	ERR_KEYSET				= 467,
	ERR_CHANNELISFULL		= 471,
	ERR_UNKNOWNMODE			= 472,
	ERR_INVITEONLYCHAN		= 473,
	ERR_BANNEDFROMCHAN		= 474,
	ERR_BADCHANNELKEY		= 475,
	ERR_NOPRIVILEGES		= 481,
	ERR_CHANOPRIVSNEEDED	= 482,
	ERR_CANTKILLSERVER		= 483,
	ERR_NOOPERHOST			= 491,
	ERR_UMODEUNKNOWNFLAG	= 501,
	ERR_USERSDONTMATCH		= 502
};

enum eReply
{
	/// Server-client replies
	RPL_WELCOME			= 001,
	RPL_YOURHOST		= 002,
	RPL_CREATED			= 003,
	RPL_MYINFO			= 004,
	RPL_BOUNCE			= 005,

	/// Replies to commands
	RPL_TRACELINK		= 200,
	RPL_TRACECONNECTING	= 201,
	RPL_TRACEHANDSHAKE	= 202,
	RPL_TRACEUNKNOWN	= 203,
	RPL_TRACEOPERATOR	= 204,
	RPL_TRACEUSER		= 205,
	RPL_TRACESERVER		= 206,
	RPL_TRACENEWTYPE	= 208,
	RPL_STATSLINKINFO	= 211,
	RPL_STATSCOMMANDS	= 212,
	RPL_STATSCLINE		= 213,
	RPL_STATSNLINE		= 214,
	RPL_STATSILINE		= 215,
	RPL_STATSKLINE		= 216,
	RPL_STATSYLINE		= 218,
	RPL_ENDOFSTATS		= 219,
	RPL_UMODEIS			= 221,
	RPL_STATSLLINE		= 241,
	RPL_STATSUPTIME		= 242,
	RPL_STATSOLINE		= 243,
	RPL_STATSHLINE		= 244,
	RPL_LUSERCLIENT		= 251,
	RPL_LUSEROP			= 252,
	RPL_LUSERUNKNOWN	= 253,
	RPL_LUSERCHANNELS	= 254,
	RPL_LUSERME			= 255,
	RPL_ADMINME			= 256,
	RPL_ADMINLOC1		= 257,
	RPL_ADMINLOC2		= 258,
	RPL_ADMINEMAIL		= 259,
	RPL_TRACELOG		= 261,
	RPL_NONE			= 300,
	RPL_AWAY			= 301,
	RPL_USERHOST		= 302,
	RPL_ISON			= 303,
	RPL_UNAWAY			= 305,
	RPL_NOWAWAY			= 306,
	RPL_WHOISUSER		= 311,
	RPL_WHOISSERVER		= 312,
	RPL_WHOISOPERATOR	= 313,
	RPL_WHOWASUSER		= 314,
	RPL_ENDOFWHO		= 315,
	RPL_WHOISIDLE		= 317,
	RPL_ENDOFWHOIS		= 318,
	RPL_WHOISCHANNELS	= 319,
	RPL_LISTSTART		= 321,
	RPL_LIST			= 322,
	RPL_LISTEND			= 323,
	RPL_CHANNELMODEIS	= 324,
	RPL_NOTOPIC			= 331,
	RPL_TOPIC			= 332,
	RPL_INVITING		= 341,
	RPL_SUMMONIN		= 342,
	RPL_VERSION			= 351,
	RPL_WHOREPLY		= 352,
	RPL_NAMREPLY		= 353,
	RPL_LINKS			= 364,
	RPL_ENDOFLINK		= 365,
	RPL_ENDOFNAME		= 366,
	RPL_BANLIST			= 367,
	RPL_ENDOFBANLIST	= 368,
	RPL_ENDOFWHOWAS		= 369,
	RPL_INFO 		 	= 371,
	RPL_MOTD 		 	= 372,
	RPL_ENDOFINFO		= 374,
	RPL_MOTDSTART		= 375,
	RPL_ENDOFMOTD		= 376,
	RPL_YOUREOPER		= 381,
	RPL_REHASHIN		= 382,
	RPL_TIME 		 	= 391,
	RPL_USERSSTAR		= 392,
	RPL_USERS			= 393,
	RPL_ENDOFUSERS		= 394,
	RPL_NOUSERS			= 395
};

#define DOMAIN	"server-domain"
#define WELCOME	"welcome-message"
#define PING_T	"ping-timeout"
#define CONN_T	"connection-timeout"
//#define PASS	"server-password"

/// Config
void		remove_comment		(std::string& str);
void		string_trim			(std::string& str, const std::string& trim_symbols);
std::string	get_config_value	(const std::string& path, const std::string& setting);
void		check_config		(const std::string& path);

///	System messages
std::string	sys_msg				(const std::string& emoji, const std::string& str);
std::string	sys_msg				(const std::string& emoji, const std::string& str
									, const std::string& white_str);
std::string	sys_msg				(const std::string& emoji, const std::string& str
									, const std::string& white_str, const std::string& ending);
std::string	sys_msg				(const std::string& emoji, const std::string& str
									, const std::string& white_str, const std::string& str2
									, const std::string& ending);

/// Other
bool		is_a_valid_nick		(const std::string& nick);
int			str_to_int			(const std::string& str);
std::string	rpl_code_to_str		(const eReply code);
std::string	rpl_code_to_str		(const eError code);

#endif
