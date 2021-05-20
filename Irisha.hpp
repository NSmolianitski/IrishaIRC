//
// Created by Parfait Kentaurus on 5/6/21.
//

#ifndef FT_IRC_IRISHA_HPP
#define FT_IRC_IRISHA_HPP

#include "AConnection.hpp"
#include "User.hpp"
#include "Server.hpp"
#include "utils.hpp"

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <deque>
#include <list>

#include <unistd.h>
#include <netinet/in.h>

#define CONFIG_PATH "irisha.conf"
#define NO_PREFIX	""
class Channel;
struct Command
{
	std::string					prefix_;
	std::string					command_;
	std::vector<std::string>	arguments_;
};

enum eResult
{
	R_SUCCESS,
	R_FAILURE
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

	typedef eResult (Irisha::*func)(const int sock);

	int			listener_;
	sockaddr_in	address_;
	char		buff_[512];
	fd_set		all_fds_;
	fd_set		read_fds_;
	fd_set		serv_fds_;
	int			max_fd_;
    Command		cmd_;			// Struct for parsed command
	std::string	host_name_;		// Host server. Need when this server connected to other.
	std::string	password_;		// Password for clients and servers connection to connect this server
	time_t		launch_time_;	// Server launch time

	std::map<std::string, AConnection*>		connections_;	// Server and client connections
	std::map<std::string, func>				commands_;		// IRC commands
    std::map<std::string ,Channel*>          channels_;

	/// Configuration members
	std::string	domain_;        // Server name
	std::string	welcome_;       // Welcome message
	int			ping_timeout_;  // How often server sends PING command
	int			conn_timeout_;	// Seconds without respond until disconnection

	std::list<Irisha::RegForm*>::iterator	expecting_registration(int i, std::list<RegForm*>& reg_expect);
	int										register_connection	(std::list<RegForm*>::iterator rf);

	/// Useful typedefs
	typedef std::map<std::string, AConnection*>::iterator		con_it;
	typedef std::map<std::string, AConnection*>::const_iterator	con_const_it;

	/// Initialization
	void			prepare_commands	();
	void			launch				();
	void 			init				(int port);
	void			apply_config		(const std::string& path);
	void			loop				();

	/// Connections
	int				accept_connection	();
	void			handle_disconnection(const int sock);
	void			handle_command		(const int sock);
	AConnection*	find_connection		(const int sock) const;
	void			ping_connections	(time_t& last_ping);

	/// Users
	void			add_user			(const int sock, const std::string& nick);
	void			add_user			();
	void			remove_user			(const std::string& nick);
	User*			find_user			(const std::string& nick) const;
	User*			find_user			(const int sock) const;

	/// Servers
	Server*			find_server			(const std::string& name) const;
	Server*			find_server			(const int sock) const;

	/// Utils
	void			send_msg			(int sock, const std::string& prefix, const std::string& msg) const;
	void			send_rpl_msg		(int sock, eReply rpl, const std::string& prefix, const std::string& msg) const;
	void			send_rpl_msg		(int sock, eError rpl, const std::string& prefix, const std::string& msg) const;
	void			send_servers		(const std::string& prefix, const std::string& msg) const;
	void			send_servers		(const std::string& prefix, const std::string& msg, const int sock) const;
	void			send_everyone		(const std::string& prefix, const std::string& msg) const;
	std::string 	get_msg				(int sock);
	void			print_info			() const;
	std::string		connection_name		(const int sock) const;

	/// IRC commands
	eResult			NICK				(const int sock);
	eResult			USER				(const int sock);
	eResult			PASS				(const int sock);
	eResult			SERVER				(const int sock);
	eResult			PING				(const int sock);
	eResult			PONG				(const int sock);
	eResult			QUIT				(const int sock);
    eResult         JOIN                (const int sock);
    eResult         MODE                (const int sock);
    eResult         PART                (const int sock);
    eResult         TOPIC               (const int sock);
    eResult         PRIVMSG             (const int sock);
	eResult			TIME				(const int sock);
	eResult			NAMES				(const int sock);
	eResult			LIST				(const int sock);
	eResult			INVITE				(const int sock);

	/// IRC commands utils
	void        send_channel    (Channel *channel, std::string msg, std::string prefix);
    void        send_channel(Channel *channel, std::string msg, std::string prefix, int sock);
	int         check_mode_channel(const Channel* channel, const int sock, std::list<std::string>& arr_key, std::string& arr_channel);
	eResult			NICK_user			(User* const connection, const int sock, const std::string& new_nick);
	eResult			NICK_server			(const std::string& new_nick);
	std::string		createPASSmsg		(std::string password);
	std::string		createSERVERmsg		();

	/// Error replies
	void			err_nosuchserver	(const int sock, const std::string& server_name);
	void			err_nosuchnick		(const int sock, const std::string& nick);
	void			err_nosuchchannel	(const int sock, const std::string& nick);

	/// Common Replies
	void			rpl_time			(const int sock, const std::string& server, const std::string& local_time);

	/// Unused constructors
	Irisha				() {};
	Irisha				(const Irisha& other) {};
	Irisha& operator=	(const Irisha& other) { return *this; };

public:
	explicit Irisha	(int port);
	Irisha			(int port, const std::string& password);
	Irisha			(const std::string& host_name, int network_port, const std::string& network_password
		   						, int port, const std::string& password);
	~Irisha			();


	/// ‼️ ⚠️ DEVELOPMENT UTILS (REMOVE OR COMMENT WHEN PROJECT IS READY) ⚠️ ‼️ //! TODO: DEV -> REMOVE ///
	enum ePrintMode
	{
		PM_LINE,
		PM_LIST,
		PM_ALL
	};

	friend void	sending_loop(const Irisha* server);
	void		print_cmd	(ePrintMode mode, const int sock) const;
	void		user_info	(const std::string& nick) const;
	void		print_user_list		() const;
	void		send_input_msg		(int sock) const;
	/// ‼️ ⚠️ END OF DEVELOPMENT UTILS ⚠️ ‼️ //! TODO: DEV -> REMOVE //////////////////////////////////////
};

/// ‼️ ⚠️ DEVELOPMENT UTILS (REMOVE OR COMMENT WHEN PROJECT IS READY) ⚠️ ‼️ //! TODO: DEV -> REMOVE ///////
void sending_loop(const Irisha* server);
/// ‼️ ⚠️ END OF DEVELOPMENT UTILS ⚠️ ‼️ //! TODO: DEV -> REMOVE //////////////////////////////////////////

#endif //FT_IRC_IRISHA_HPP
