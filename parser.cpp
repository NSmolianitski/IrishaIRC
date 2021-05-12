#include "Server.hpp"
#include "parser.hpp"

void 	parseMsg(std::string msg, t_msg *msgStruct)
{
	// std::deque<std::string> array;
	// std::istringstream is(msg);
	// std::string s;

	// msgStruct->pref.clear();
	// msgStruct->command.clear();
	// msgStruct->param.clear();
	// while (std::getline(is, s, ' '))
	// 	array.push_back(s);
	// if (!array.empty()) {
	// 	if (array[0][0] == ':') {
	// 		msgStruct->pref.append(array[0].erase(0, 1));
	// 		std::cout << "PREFIX = " << msgStruct->pref << std::endl;
	// 		array.pop_front();
	// 	}
	// 	msgStruct->command.append(array[0]);
	// 	std::cout << "COMMAND = " << msgStruct->command << std::endl;
	// 	array.pop_front();
	// 	while (array.size() > 0) {
	// 		msgStruct->param.push_back(array[0]);
	// 		array.pop_front();
	// 	}
	// 	std::vector<std::string>::iterator itr = msgStruct->param.begin();
	// 	std::cout << "PARAMS = ";
	// 	while (itr != msgStruct->param.end()) {
	// 		std::cout << *itr << " ";
	// 		itr++;
	// 	}
	// }
	// std::cout << msg.size() << std::endl;
}
