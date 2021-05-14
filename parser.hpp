#pragma once
#include <vector>
#include <deque>
#include <string>
#include "Irisha.hpp"

void 	parse_msg(const std::string& msg, t_msg *msgStruct);
void    parse_arr_msg(std::deque<std::string> *arr_msg, const std::string& client_msg);
void    parse_argv(int argc, char *argv[], std::string& host, int& port_network, std::string& password_network, int& port, std::string& password);