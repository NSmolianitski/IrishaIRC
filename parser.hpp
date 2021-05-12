#pragma once
#include <vector>
#include <deque>
#include <string>
//typedef struct s_msg{
//	std::string pref;
//	std::string command;
//	std::vector<std::string> param;
//}				t_msg;
void 	parseMsg(std::string msg, t_msg *msgStruct);