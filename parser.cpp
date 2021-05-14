#include "parser.hpp"
#include "utils.hpp"

void 	parse_msg(const std::string& msg, Command& cmd)
{
    std::deque<std::string> array;
    std::istringstream is(msg);
    std::string s;

    cmd.prefix.clear();
    cmd.command.clear();
    cmd.arguments.clear();
    while (std::getline(is, s, ' '))
        array.push_back(s);
    if (!array.empty()) {
        if (array[0][0] == ':') {
            cmd.prefix.append(array[0].erase(0, 1));
            std::cout << "PREFIX = " << cmd.prefix << std::endl;
            array.pop_front();
        }
        cmd.command.append(array[0]);
        std::cout << "COMMAND = " << cmd.command << std::endl;
        array.pop_front();
        while (array.size() > 0) {
            cmd.arguments.push_back(array[0]);
            array.pop_front();
        }
        std::vector<std::string>::iterator itr = cmd.arguments.begin();
        std::cout << "ARGUMENTS = ";
        while (itr != cmd.arguments.end()) {
            std::cout << *itr << ITALIC PURPLE " | " CLR;
            itr++;
        }
    }
    std::cout << std::endl;
}

void parse_arr_msg(std::deque<std::string>& arr_msg, const std::string& client_msg)
{
    std::string s;
    std::istringstream is(client_msg);


    while (std::getline(is, s, '\r'))
        arr_msg.push_back(s);
    if (!arr_msg.empty())
    {
        arr_msg.pop_back();
        std::deque<std::string>::iterator itr = arr_msg.begin();
        while (itr != arr_msg.end())
        {
            if ((*itr).find('\n') != (*itr).npos)
                (*itr).erase((*itr).find('\n'), 1);
            itr++;
        }
    }
}