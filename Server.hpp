//
// Created by Ullhor Greyiron on 5/13/21.
//

#ifndef FT_IRC_SERVER_HPP
#define FT_IRC_SERVER_HPP

#include "AConnection.hpp"
#include "utils.hpp"
#include <string>

class Server	:public AConnection
{
public:
	Server(std::string name, int socket, int hopcount, int token, int source_socket);
	~Server();

	const std::string&	name();
	int 				token();
private:
	std::string name_;
	int 		token_;

	Server();
	Server(const Server& server);
	Server& operator=(const Server& rh);
};


#endif //FT_IRC_SERVER_HPP
