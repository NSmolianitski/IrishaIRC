//
// Created by Ullhor Greyiron on 5/13/21.
//

#include "Server.hpp"

Server::Server(std::string name, int socket, int hopcount)
		: AConnection(socket, T_SERVER, hopcount)
{
	name_ = name;
}

Server::Server() :AConnection(-1, T_SERVER, -1) {}

Server::~Server() {}

Server::Server(const Server &server) : AConnection(-1, T_SERVER, -1) {}

const std::string& Server::name()	{ return name_; }

Server& Server::operator=(const Server &rh) { return *this; }