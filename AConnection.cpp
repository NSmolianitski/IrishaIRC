
#include "AConnection.hpp"

#include <ctime>

AConnection::AConnection() {}
AConnection::AConnection(int socket, eType type, int hopcount, int source_socket, int token)
	: socket_(socket), type_(type), hopcount_(hopcount), last_msg_time_(time(nullptr)), source_socket_(source_socket),
		token_(token) {}
AConnection::~AConnection() {}

int				AConnection::socket				() const { return socket_; }
eType			AConnection::type				() const { return type_; }
int				AConnection::hopcount			() const { return hopcount_; }
void			AConnection::update_time		() { last_msg_time_ = time(nullptr); }
std::string&	AConnection::buff				() { return buff_; }
int				AConnection::token				() const { return token_; }

double			AConnection::last_msg_time		() const
{
	return difftime(time(nullptr), last_msg_time_);
}
