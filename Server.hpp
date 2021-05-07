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
	int					socket_;
	struct sockaddr_in	address_;

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

	int		accept_client	() const;
	void	send_msg		(int client_socket, const std::string& msg) const;
	Signal	send_input_msg	(int client_socket) const;

};



#endif //FT_IRC_SERVER_HPP
