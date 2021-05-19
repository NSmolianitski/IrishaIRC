//
// Created by Parfait Kentaurus on 5/12/21.
//

#include "AConnection.hpp"

#include <ctime>

AConnection::AConnection() {}
AConnection::AConnection(int socket, eType type, int hopcount)
	: socket_(socket), type_(type), hopcount_(hopcount), last_msg_time_(time(nullptr)) {}
AConnection::~AConnection() {}

int				AConnection::socket				() const { return socket_; }
eType			AConnection::type				() const { return type_; }
int				AConnection::hopcount			() const { return hopcount_; }
void			AConnection::update_time		() { last_msg_time_ = time(nullptr); }
std::string&	AConnection::buff				() { return buff_; }

double			AConnection::last_msg_time		() const
{
	return difftime(time(nullptr), last_msg_time_);
}
