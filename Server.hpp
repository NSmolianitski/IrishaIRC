//
// Created by Parfait Kentaurus on 5/6/21.
//

#ifndef FT_IRC_SERVER_HPP
#define FT_IRC_SERVER_HPP

#include <iostream>
#include <vector>

#include <unistd.h>
#include <netinet/in.h>

class Server
{
private:
	int					listener_;
	struct sockaddr_in	address_;
	char				buff_[512];
	fd_set				client_fds_;
	fd_set				read_fds_;
	int					max_fd_;

	void	launch			();

	/// Unused constructors
	Server() {};
	Server(const Server& other) {};
	Server& operator= (const Server& other) { return *this; };

public:
	struct Command
	{
		std::string					sender;
		std::string					command;
		std::vector<std::string>	arguments;
	};

	explicit Server(int port);
	~Server();

	void		handle_disconnection(int client_socket);
	void		send_msg			(int client_socket, const std::string& msg) const;
	void		send_input_msg		(int client_socket) const;
	std::string get_msg				(int client_socket);
	void		loop				();

	/// Server-client
	int			accept_client		();
	void		nick				();

	friend void	sending_loop		(const Server* server); //! TODO: REMOVE //////////////////////////////////////
};



#endif //FT_IRC_SERVER_HPP
