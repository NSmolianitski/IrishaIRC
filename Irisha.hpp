//
// Created by Parfait Kentaurus on 5/6/21.
//

#ifndef FT_IRC_IRISHA_HPP
#define FT_IRC_IRISHA_HPP

#include "AConnection.hpp"

#include <iostream>
#include <map>
#include <vector>
#include <deque>

#include <unistd.h>
#include <netinet/in.h>
#include <list>

struct Command
{
	std::string					prefix;
	std::string					command;
	std::vector<std::string>	arguments;
};

#define CONFIG_PATH "irisha.conf"
#define NO_PREFIX	""

class User;

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
    Command				        cmd_; // Struct parsing mess
	std::string 				host_name_;		// Host server. Need when this server connected to other.
	std::string					password_;		// Password for clients and servers connection to connect this server
	std::map<int, AConnection*>	connections_;	// Server and client connections

	void	launch();
	void 	init(int port);

	/// Unused constructors
	Irisha() {};
	Irisha(const Irisha& other) {};
	Irisha& operator= (const Irisha& other) { return *this; };

public:
	typedef struct RegForm
	{
		int		fd_;
		bool	pass_received_;

		RegForm(int fd)
		{
			fd_ = fd;
			pass_received_ = false;
		}

		static std::list<RegForm>::iterator expecting_registration(int i, std::list<RegForm>& reg_expect)
		{
			std::list<RegForm>::iterator it;
			for (it = reg_expect.begin(); it != reg_expect.end(); it++)
			{
				if (it->fd_ == i)
					return it;
			}
			return (reg_expect.end());
		}

	}				RegForm;

	explicit Irisha(int port);
	Irisha(int port, const std::string& password);
	Irisha(const std::string& host_name, int network_port, const std::string& network_password,
		   int port, const std::string& password);
	~Irisha();

	/// Initialization
	void		apply_config		(const std::string& path);
	void		loop				();

	/// Connections
	int			accept_connection	();
	int			register_connection	(std::list<RegForm>::iterator rf, Command cmd);
	void		handle_disconnection(int client_socket);

	/// Users
	void		add_user			(int sock, const std::string& nick);
	void		remove_user			(const std::string& nick);
	User*		find_user			(const std::string& nick) const;

	/// Utils
	void		send_msg			(int sock, const std::string& prefix, const std::string& msg) const;
	void		send_input_msg		(int client_socket) const;
	std::string get_msg				(int client_socket);
	void		print_info			();
	friend void	sending_loop		(const Irisha* server); //! TODO: REMOVE //////////////////////////////////////

	/// Servers
//	Server*		find_server			(const std::string& nick) const;

	/// IRC Commands
	void		nick				(const Command& cmd, const int socket);

	std::string createPASSmsg		(std::string password);
	std::string createSERVERmsg		();
};



#endif //FT_IRC_IRISHA_HPP
