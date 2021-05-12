//
// Created by Parfait Kentaurus on 5/6/21.
//

#ifndef FT_IRC_IRISHA_HPP
#define FT_IRC_IRISHA_HPP

#include <iostream>

#include <unistd.h>
#include <netinet/in.h>
#include <list>

class Irisha
{
private:
	int					listener_;
	//int 				speaker_;
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
	Irisha() {};
	Irisha(const Irisha& other) {};
	Irisha& operator= (const Irisha& other) { return *this; };

public:
	enum Signal /// TODO: ???
	{
		S_NOSIGNAL,
		S_MSG_SENT,
		S_SHUTDOWN
	};

	typedef struct RegForm
	{
		int		fd_;
		bool	pass_received_;

		RegForm(int fd)
		{
			fd_ = fd;
			pass_received_ = false;
		}

		static RegForm* expecting_registration(int i, std::list<RegForm> reg_expect)
		{
			std::list<RegForm>::iterator it;
			for (it = reg_expect.begin(); it != reg_expect.end(); it++)
			{
				if (it->fd_ == i)
					return &(*it);
			}
			return 0;
		}

		friend bool operator==(RegForm x, RegForm y)
		{
			return (x.fd_ == y.fd_);
		}
	}				RegForm;

	explicit Irisha(int port);
	Irisha(int port, const std::string& password);
	Irisha(const std::string& host_name, int network_port, const std::string& network_password,
		   int port, const std::string& password);
	~Irisha();

	int			accept_connection	();
	void		handle_disconnection(int client_socket);
	void		send_msg			(int client_socket, const std::string& msg) const;
	Signal		send_input_msg		(int client_socket) const;
	std::string get_msg				(int client_socket);
	void		loop				();
	std::string createPASSmsg		(std::string password);
	std::string createSERVERmsg		();

	friend void	sending_loop		(const Irisha* server); //! TODO: REMOVE //////////////////////////////////////
};



#endif //FT_IRC_IRISHA_HPP
