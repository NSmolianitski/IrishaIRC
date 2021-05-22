
#include "Server.hpp"

Server::Server(std::string name, int socket, int hopcount, int token, int source_socket)
		: AConnection(socket, T_SERVER, hopcount, source_socket, token)
{
	name_ = name;
}

Server::Server() :AConnection(-1, T_SERVER, -1, U_EXTERNAL_CONNECTION, 0) {}

Server::~Server() {}

Server::Server(const Server &server) : AConnection(-1, T_SERVER, -1, U_EXTERNAL_CONNECTION, 0) {}

const std::string& Server::name()	{ return name_; }

Server& Server::operator=(const Server &rh) { return *this; }