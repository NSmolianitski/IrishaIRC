//
// Created by Parfait Kentaurus on 5/6/21.
//

#ifndef FT_IRC_SERVER_HPP
#define FT_IRC_SERVER_HPP

#include <unistd.h>
#include <netinet/in.h>

#include <iostream>

class Server
{
private:
	int					listener_;
	struct sockaddr_in	address_;
	char				buff_[510];

	void	launch			();

	/// Unused constructors
	Server() {};
	Server(const Server& other) {};
	Server& operator= (const Server& other) { return *this; };

public:
	explicit Server(int port);
	~Server();

	enum Signal
	{
		S_NOSIGNAL,
		S_MSG_SENT,
		S_SHUTDOWN
	};

	int			accept_client	() const;
	void		send_msg		(int client_socket, const std::string& msg) const;
	Signal		send_input_msg	(int client_socket) const;
	std::string get_msg			(int client_socket);

	/// HAVE TO BE DELETED
	int get_listener() const { return listener_; }
};



#endif //FT_IRC_SERVER_HPP
