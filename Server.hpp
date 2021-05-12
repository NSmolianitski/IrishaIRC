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
	int 				speaker_;
	struct sockaddr_in	address_;
	char				buff_[512];
	fd_set				all_fds_;
	fd_set				read_fds_;
	fd_set				serv_fds_;
	int					max_fd_;
	std::string 		host_name_;	// Host server. Need when this server connected to other.
	std::string			password_;	// Password for clients and servers connection to connect this server

	void	launch();
	void 	init(int port);
	/// Unused constructors
	Server() {};
	Server(const Server& other) {};
	Server& operator= (const Server& other) { return *this; };

public:
	struct CommandLine
	{
		std::string					sender;
		std::string					command;
		std::vector<std::string>	arguments;
	};

	explicit Server(int port);
	Server(int port, const std::string& password);
	Server(const std::string& host_name, int network_port, const std::string& network_password,
		   int port, const std::string& password);
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
