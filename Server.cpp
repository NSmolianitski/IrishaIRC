//
// Created by Ullhor Greyiron on 5/13/21.
//

#include "Server.hpp"

Server::Server(std::string name, int socket, int hopcount)
		: AConnection(socket, T_SERVER, hopcount)
{
	name_ = name;
}

Server::Server() {}

Server::~Server() {}

Server::Server(const Server &server) {}

Server & Server::operator=(const Server &rh) {}