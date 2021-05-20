//
// Created by Ullhor Greyiron on 5/13/21.
//

#include "Server.hpp"

Server::Server(std::string name, int socket, int hopcount, int token, int source_socket)
		: AConnection(socket, T_SERVER, hopcount, source_socket)
{
	name_ = name;
	token_ = token;
}

Server::Server() :AConnection(-1, T_SERVER, -1, U_EXTERNAL_CONNECTION) {}

Server::~Server() {}

Server::Server(const Server &server) : AConnection(-1, T_SERVER, -1, U_EXTERNAL_CONNECTION) {}

const std::string& Server::name()	{ return name_; }

int Server::token()	{ return token_; }

Server& Server::operator=(const Server &rh) { return *this; }