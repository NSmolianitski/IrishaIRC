
#include "AConnection.hpp"

#include <ctime>

AConnection::AConnection() {}
AConnection::AConnection(int socket, eType type, int hopcount, int source_socket, int token)
		: socket_(socket), type_(type), hopcount_(hopcount), source_socket_(source_socket), last_msg_time_(time(nullptr)),
		  token_(token), launch_time_(time(0)) {}
AConnection::~AConnection() {}

int				AConnection::socket				() const { return socket_; }
eType			AConnection::type				() const { return type_; }
int				AConnection::hopcount			() const { return hopcount_; }
void			AConnection::update_time		() { last_msg_time_ = time(nullptr); }
std::string&	AConnection::buff				() { return buff_; }
int				AConnection::token				() const { return token_; }
int				AConnection::source_socket		() const { return source_socket_; }

double			AConnection::last_msg_time		() const
{
	return difftime(time(nullptr), last_msg_time_);
}

time_t			AConnection::launch_time() const { return launch_time_; }

