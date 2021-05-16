//
// Created by Parfait Kentaurus on 5/6/21.
//

#ifndef FT_IRC_IRISHA_HPP
#define FT_IRC_IRISHA_HPP

#include "AConnection.hpp"
#include "User.hpp"

#include <iostream>
#include <map>
#include <vector>
#include <deque>
#include <list>

#include <unistd.h>
#include <netinet/in.h>

#define CONFIG_PATH "irisha.conf"
#define NO_PREFIX	""

struct Command
{
	std::string					prefix_;
	std::string					command_;
	std::vector<std::string>	arguments_;
};

enum CmdResult
{
	CMD_SUCCESS,
	CMD_FAILURE
};

class Irisha
{
private:
	struct RegForm
	{
		int		socket_;
		bool	pass_received_;

		explicit RegForm(int sock)
		{
			socket_ = sock;
			pass_received_ = false;
		}
	};

	typedef CmdResult (Irisha::*func)(const int sock);

	std::string	domain_;
	int			listener_;
	sockaddr_in	address_;
	char		buff_[512];
	fd_set		all_fds_;
	fd_set		read_fds_;
	fd_set		serv_fds_;
	int			max_fd_;
    Command		cmd_;		// Struct for parsed command
	std::string	host_name_;	// Host server. Need when this server connected to other.
	std::string	password_;	// Password for clients and servers connection to connect this server

	std::map<int, AConnection*>	connections_;	// Server and client connections
	std::map<std::string, func>	commands_;		// IRC commands

	std::list<Irisha::RegForm*>::iterator	expecting_registration(int i, std::list<RegForm*>& reg_expect);
	int										register_connection	(std::list<RegForm*>::iterator rf);

	/// Useful typedefs
	typedef std::map<int, AConnection*>::iterator		con_it;
	typedef std::map<int, AConnection*>::const_iterator	con_const_it;

	/// Initialization
	void		prepare_commands	();
	void		launch				();
	void 		init				(int port);
	void		apply_config		(const std::string& path);
	void		loop				();

	/// Connections
	int			accept_connection	();
	void		handle_disconnection(int sock);
	void		handle_command		(int sock);

	/// Users
	void		add_user			(int sock, const std::string& nick);
	void		remove_user			(const std::string& nick);
	User*		find_user			(const std::string& nick) const;
	User*		find_user			(const int sock) const;
	void		print_user_list		(); //! TODO: remove

	/// Servers
//	Server*		find_server			(const std::string& nick) const;

	/// Utils
	void		send_msg			(int sock, const std::string& prefix, const std::string& msg) const;
	void		send_input_msg		(int sock) const;
	std::string get_msg				(int sock);
	void		print_info			() const;

	/// IRC commands
	CmdResult	NICK	(const int sock);
	CmdResult	PASS	(const int sock);
	CmdResult	SERVER	(const int sock);
	CmdResult	PING	(const int sock);
	CmdResult	PONG	(const int sock);

	/// IRC commands utils
	CmdResult	change_nick		(User* connection, const int sock, const std::string& new_nick);
	std::string	createPASSmsg	(std::string password);
	std::string	createSERVERmsg	();

	/// Unused constructors
	Irisha() {};
	Irisha(const Irisha& other) {};
	Irisha& operator= (const Irisha& other) { return *this; };

public:
	explicit Irisha(int port);
	Irisha(int port, const std::string& password);
	Irisha(const std::string& host_name, int network_port, const std::string& network_password,
		   int port, const std::string& password);
	~Irisha();


	/// ‼️ ⚠️ DEVELOPMENT UTILS (REMOVE OR COMMENT WHEN PROJECT IS READY) ⚠️ ‼️ //! TODO: DEV -> REMOVE ///
	enum PrintMode
	{
		PM_LINE,
		PM_LIST,
		PM_ALL
	};

	friend void	sending_loop(const Irisha* server);
	void		print_cmd	(PrintMode mode, const int sock) const;
	/// ‼️ ⚠️ END OF DEVELOPMENT UTILS ⚠️ ‼️ //! TODO: DEV -> REMOVE //////////////////////////////////////
};

/// ‼️ ⚠️ DEVELOPMENT UTILS (REMOVE OR COMMENT WHEN PROJECT IS READY) ⚠️ ‼️ //! TODO: DEV -> REMOVE ///////
void sending_loop(const Irisha* server);
/// ‼️ ⚠️ END OF DEVELOPMENT UTILS ⚠️ ‼️ //! TODO: DEV -> REMOVE //////////////////////////////////////////

#endif //FT_IRC_IRISHA_HPP
