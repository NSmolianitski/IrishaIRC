//
// Created by Parfait Kentaurus on 5/6/21.
//

#ifndef FT_IRC_SERVER_HPP
#define FT_IRC_SERVER_HPP

#include <iostream>

#include <unistd.h>
#include <netinet/in.h>

class Server
{
private:
	int					listener_;
	struct sockaddr_in	address_;
	char				buff_[510];
	fd_set				client_fds_;
	fd_set				read_fds_;
	int					max_fd_;

	void	launch			();

	/// Unused constructors
	Server() {};
	Server(const Server& other) {};
	Server& operator= (const Server& other) { return *this; };

public:
	explicit Server(int port);
	~Server();

	enum Signal /// ???
	{
		S_NOSIGNAL,
		S_MSG_SENT,
		S_SHUTDOWN
	};

	int			accept_client		();
	void		handle_disconnection(int client_socket);
	void		send_msg			(int client_socket, const std::string& msg) const;
	Signal		send_input_msg		(int client_socket) const;
	std::string get_msg				(int client_socket);
	void		loop				();
};



#endif //FT_IRC_SERVER_HPP
