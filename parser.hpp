#pragma once
#include <vector>
#include <deque>
#include <string>
#include <sstream>
#include "Irisha.hpp"

void 	parse_msg(const std::string& msg, Command& cmd);
void    parse_arr_msg(std::deque<std::string>& arr_msg, const std::string& client_msg);
void    parse_argv(int argc, char *argv[], std::string& host, int& port_network, std::string& password_network, int& port, std::string& password);