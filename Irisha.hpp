//
// Created by Parfait Kentaurus on 5/6/21.
//

#ifndef FT_IRC_IRISHA_HPP
#define FT_IRC_IRISHA_HPP

#include "AConnection.hpp"

#include <iostream>
#include <map>
#include <vector>

#include <unistd.h>
#include <netinet/in.h>
#include <list>

#define CONFIG_PATH "irisha.conf"

class Irisha
{
private:
	std::string					domain_;
	int							listener_;
	struct sockaddr_in			address_;
	char						buff_[512];
	fd_set						all_fds_;
	fd_set						read_fds_;
	fd_set						serv_fds_;
	int							max_fd_;
	std::string 				host_name_;		// Host server. Need when this server connected to other.
	std::string					password_;		// Password for clients and servers to connect this server
	std::map<int, AConnection*>	connections_;	// Server and client connections

	typedef struct RegForm
	{
		int		fd_;
		bool	pass_received_;

		RegForm(int fd)
		{
			fd_ = fd;
			pass_received_ = false;
		}
	}				RegForm;

	struct Command
	{
		std::string					sender;
		std::string					command;
		std::vector<std::string>	arguments;
	};

	void	launch();
	void 	init(int port);

	/// Unused constructors
	Irisha() {};
	Irisha(const Irisha& other) {};
	Irisha& operator= (const Irisha& other) { return *this; };

	std::list<Irisha::RegForm>::iterator	expecting_registration(int i, std::list<RegForm>& reg_expect);
	int										register_connection	(std::list<RegForm>::iterator rf, Command& cmd);

	int 									cmd_PASS(int fd, const Command& cmd);
	int 									cmd_SERVER(int fd, const Command& cmd);

public:

	explicit Irisha(int port);
	Irisha(int port, const std::string& password);
	Irisha(const std::string& host_name, int network_port, const std::string& network_password,
		   int port, const std::string& password);
	~Irisha();

	void		apply_config		(const std::string& path);
	void		print_info			();
	int			accept_connection	();
	std::string createPASSmsg		(std::string password);
	std::string createSERVERmsg		();
	void		handle_disconnection(int client_socket);
	void		send_msg			(int client_socket, const std::string& msg) const;
	void		send_input_msg		(int client_socket) const;
	std::string get_msg				(int client_socket);
	void		loop				();


	/// Server-client
	int			accept_client		();
	void		nick				(const Command& cmd);

	friend void	sending_loop		(const Irisha* server); //! TODO: REMOVE //////////////////////////////////////
};



#endif //FT_IRC_IRISHA_HPP