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
	int 				speaker_;
	struct sockaddr_in	address_;
	char				buff_[512];
	fd_set				all_fds_;
	fd_set				read_fds_;
	fd_set				serv_fds_;
	int					max_fd_;
	std::string 		hostName_; // host server. Need when this server connected to other.
	std::string			password_; //password for clients and servers connection to connect this server

	void	launch();
	void 	init(int port);
	/// Unused constructors
	Server() {};
	Server(const Server& other) {};
	Server& operator= (const Server& other) { return *this; };

public:
	explicit Server(int port);
	Server(int port, const std::string& password);
	Server(const std::string& hostName, int portNetwork, const std::string& passwordNetwork,
		   int port, const std::string& password);
	~Server();

	enum Signal /// TODO: ???
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

	friend void	sending_loop		(const Server* server); //! TODO: REMOVE //////////////////////////////////////
};



#endif //FT_IRC_SERVER_HPP
