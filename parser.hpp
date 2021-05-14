#pragma once

#include "Irisha.hpp"

#include <vector>
#include <deque>
#include <string>
#include <sstream>

void 	parse_msg(const std::string& msg, Command& cmd);
void    parse_arr_msg(std::deque<std::string>& arr_msg, const std::string& client_msg);