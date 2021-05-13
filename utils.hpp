//
// Created by Parfait Kentaurus on 5/9/21.
//

#ifndef FT_IRC_UTILS_HPP
#define FT_IRC_UTILS_HPP

///	Font-style and colors
#define RED		"\033[31m"
#define GREEN	"\033[32m"
#define YELLOW	"\033[33m"
#define BLUE	"\033[34m"
#define WHITE	"\033[97m"
#define PURPLE	"\033[35m"

#define CLR		"\033[0m"
#define BOLD	"\033[1m"
#define ITALIC	"\033[3m"
#define UND		"\033[4m"

#define DOMAIN	"server-domain"
#define PASS	"server-password"

void		remove_comment		(std::string& str);
void		string_trim			(std::string& str, const std::string& trim_symbols);
std::string	get_config_value	(const std::string& path, const std::string& setting);
void		check_config		(const std::string& path);

#endif
