
#include "Server.hpp"

Server::Server(std::string name, int socket, int hopcount, int token)
		: AConnection(socket, T_SERVER, hopcount)
{
	name_ = name;
	token_ = token;
}

Server::Server() :AConnection(-1, T_SERVER, -1) {}

Server::~Server() {}

Server::Server(const Server &server) : AConnection(-1, T_SERVER, -1) {}

const std::string& Server::name()	{ return name_; }

int Server::token()	{ return token_; }

Server& Server::operator=(const Server &rh) { return *this; }