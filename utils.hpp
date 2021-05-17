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

#define DOMAIN	"server-domain"
//#define PASS	"server-password"

/// Config
void		remove_comment		(std::string& str);
void		string_trim			(std::string& str, const std::string& trim_symbols);
std::string	get_config_value	(const std::string& path, const std::string& setting);
void		check_config		(const std::string& path);

/// Other
bool		is_a_valid_nick		(const std::string& nick);

#endif
