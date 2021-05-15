//#include "Server.hpp"
#include <cstring>
#include "parser.hpp"
#include "utils.hpp"

void 	parse_msg(const std::string& msg, t_msg *msgStruct)
{
     std::deque<std::string> array;
     std::istringstream is(msg);
     std::string s;

     msgStruct->pref.clear();
     msgStruct->command.clear();
     msgStruct->param.clear();
     while (std::getline(is, s, ' '))
     	array.push_back(s);
     if (!array.empty()) {
     	if (array[0][0] == ':') {
     		msgStruct->pref.append(array[0].erase(0, 1));
     		std::cout << "PREFIX = " << msgStruct->pref << std::endl;
     		array.pop_front();
     	}
     	msgStruct->command.append(array[0]);
     	std::cout << "COMMAND = " << msgStruct->command << std::endl;
     	array.pop_front();
     	while (!array.empty()) {
     		if (array[0][0] == ':')
     			break;
     		msgStruct->param.push_back(array[0]);
     		array.pop_front();
     	}
     	s.clear();
     	while (!array.empty())
		{
     		if (!s.empty())
     			s.append(" ");
     		s.append(array[0]);
			array.pop_front();
		}
     	if (!s.empty())
			msgStruct->param.push_back(s);
     	std::vector<std::string>::iterator itr = msgStruct->param.begin();
     	std::cout << "PARAMS = ";
     	while (itr != msgStruct->param.end()) {
     		std::cout << *itr << ITALIC PURPLE " | " CLR;
     		itr++;
     	}
     }
     std::cout << std::endl;
}

void parse_arr_msg(std::deque<std::string> *arr_msg, const std::string& client_msg)
{
    std::string s;
    std::istringstream is(client_msg);

    while (std::getline(is, s, '\r'))
        arr_msg->push_back(s);
    if (arr_msg->size() > 0)
    {
        if (arr_msg->operator[](arr_msg->size() - 1) == "\n")
            arr_msg->pop_back();
        std::deque<std::string>::iterator itr = arr_msg->begin();
        while (itr != arr_msg->end())
        {
            if ((*itr).find('\n') != (*itr).npos)
                (*itr).erase((*itr).find('\n'), 1);
            itr++;
        }
    }
}

void parse_argv(int argc, char *argv[], std::string& host, int& port_network, std::string& password_network, int& port, std::string& password)
{
    std::deque<std::string> array;
    std::string s;
    if (argc < 3 || argc > 4){
        std::cout << RED "Error size arguments" CLR << std::endl;
        exit(1);
    }
    if (argc == 3){
        for (int i = 0; i < strlen(argv[1]); ++i) {
            if (isdigit(argv[1][i]) == 0){
                std::cout << RED "Error: port" CLR << std::endl;
                exit(1);
            }
        }
        if (strlen(argv[1]) > 5 || strlen(argv[1]) < 4){
            std::cout << RED "Error: size port" CLR << std::endl;
            exit(1);
        }
        port = std::atoi(argv[1]);
        password = argv[2];
    }
    else {
        std::istringstream is(argv[1]);
        while (std::getline(is, s, ':')){
            if (!s.empty())
                array.push_back(s);
        }
        if (array.size() != 3){
            std::cout << RED "Error size arguments network" CLR << std::endl;
            exit(1);
        }
        host = array[0];
        array.pop_front();
        for (int i = 0; i < array[0].length(); ++i) {
            if (isdigit(array[0][i]) == 0){
                std::cout << RED "Error: port network" CLR << std::endl;
                exit(1);
            }
        }
        if (array[0].length() > 5 || array[0].length() < 4){
            std::cout << RED "Error: size port network" CLR << std::endl;
            exit(1);
        }
        port_network = std::stoi(array[0]);
        array.pop_front();
        password_network = array[0];
        array.pop_front();
        for (int i = 0; i < strlen(argv[2]); ++i) {
            if (isdigit(argv[2][i]) == 0){
                std::cout << RED "Error: port" CLR << std::endl;
                exit(1);
            }
        }
        port = std::atoi(argv[2]);
        password = argv[3];
    }
}