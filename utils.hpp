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
#define WHITE	"\033[97m"
#define PURPLE	"\033[35m"

#define BOLD	"\033[1m"
#define ITALIC	"\033[3m"
#define CLR		"\033[0m"

bool is_a_valid_nick(const std::string& nick)
{
	if (nick.empty() || nick.size() > 9 || isnumber(nick.front()))
		return false;
	if (nick.find_first_not_of("abcdefghijklmnopqrstuvwxyz"
							   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
							   "01234567890_[]\\`^{|}-") != std::string::npos)
		return false;
	return true;
}

#endif
