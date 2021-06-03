
#ifndef FT_IRC_IRISHA_HPP
#define FT_IRC_IRISHA_HPP

#include "AConnection.hpp"
#include "User.hpp"
#include "Server.hpp"
#include "utils.hpp"

#include <unistd.h>
#include <netinet/in.h>

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <list>

#define CONFIG_PATH "irisha.conf"
#define NO_PREFIX	""

struct Command
{
	std::string                 line_;		// Whole command line
	std::string					prefix_;
	std::string					command_;
	std::vector<std::string>	arguments_;
	eType                       type_;		// Sender connection type
};

enum eResult
{
	R_SUCCESS,
	R_FAILURE
};

class Channel;

class Irisha
{
private:
	struct RegForm
	{
		int			socket_;
		bool		pass_received_;
		std::string buff_;
		time_t		connection_time_;

		explicit RegForm(int sock)
		{
			socket_ = sock;
			pass_received_ = false;
			connection_time_ = time(nullptr);
		}
	};

	typedef eResult (Irisha::*func)(const int sock);

	int			listener_;
	std::string buff_;
	sockaddr_in	address_;
	fd_set		all_fds_;
	fd_set		read_fds_;
	fd_set		serv_fds_;
	int			max_fd_;
    Command		cmd_;			// Struct for parsed command
	std::string	host_name_;		// Host server. Need when this server connected to other.
	std::string	password_;		// Password for clients and servers connection to connect this server
	time_t		launch_time_;	// Server launch time
	int 		parent_fd_;
	std::string oper_pass_;

	std::map<std::string, AConnection*>		connections_;	// Server and client connections
	std::map<std::string, func>				commands_;		// IRC commands
    std::map<std::string ,Channel*>          channels_;

	/// Configuration members
	std::string	domain_;        // Server name
	std::string	welcome_;       // Welcome message
	std::string	admin_mail_;
	std::string	admin_location_;
	std::string	admin_info_;    // Admin information
	int			ping_timeout_;  // How often server sends PING command
	int			conn_timeout_;	// Seconds without respond until disconnection
	int			reg_timeout_;	// Seconds for registration until disconnection
	eUtils		time_stamp_;	// Enabled or disabled time stamps

	std::list<Irisha::RegForm*>::iterator	expecting_registration(int i, std::list<RegForm*>& reg_expect);
	int										register_connection	(std::list<RegForm*>::iterator rf);

	/// Useful typedefs
	typedef std::map<std::string, AConnection*>::iterator		con_it;
	typedef std::map<std::string, AConnection*>::const_iterator	con_const_it;

	/// Initialization
	void			prepare_commands	();
	void			launch				();
	void 			init				(int port);
	void			loop				();

	/// Config
	void			apply_config		(const std::string& path);
	void			check_timeout_values();
	void			check_domain		();
	void			set_time_stamp		(const std::string& path);

	/// Connections
	int				accept_connection	();
	void			close_connection	(const int sock, const std::string& comment, std::list<Irisha::RegForm*>* reg_expect);
	void			handle_command		(const int sock);
	AConnection*	find_connection		(const int sock) const;
	AConnection*	find_connection		(const std::string& name) const;
	void			ping_connections	(time_t& last_ping);
	void			check_reg_timeouts	(std::list<Irisha::RegForm*>& reg_expect);
	std::string		connection_name		(const int sock) const;
	std::string		connection_name		(AConnection* connection) const;

	/// Users
	void			add_user			(const int sock, const std::string& nick);
	void			add_user			(int source_sock);
	void			remove_user			(const std::string& nick);
	void			remove_user			(User*& user);
	void			remove_server_users	(const std::string& name);
	User*			find_user			(const std::string& nick) const;
	User*			find_user			(const int sock) const;
	bool			is_user_operator	(const int sock);
    eResult         check_user          (int sock, User*& user, const std::string& nick);

	/// Servers
	void			remove_server		(const std::string& name);
	void			remove_server		(Server*& server);
	void			remove_local_server	(Server*& server);
	Server*			find_server			(const std::string& name) const;
	Server*			find_server			(const int sock) const;
	eResult			check_server		(int sock, Server*& server, const std::string& name);

	/// Utils
	std::string*		choose_buff			(int sock, std::list<Irisha::RegForm*>& reg_expect);
	std::string* 		get_msg				(int sock, std::list<Irisha::RegForm*>& reg_expect);
	std::string 		time_stamp			() const;
	RegForm*	 		find_regform		(int sock, std::list<Irisha::RegForm*>& reg_expect);
	bool				is_valid_prefix		(const int sock);
	void				send_msg			(int sock, const std::string& prefix, const std::string& msg) const;
	void				send_msg			(int sock, const std::string& msg) const;
	void				send_rpl_msg		(int sock, eReply rpl, const std::string& msg) const;
	void				send_rpl_msg		(int sock, eReply rpl, const std::string& msg
												, const std::string& target) const;
	void				send_rpl_msg		(int sock, eError rpl, const std::string& msg) const;
	void				send_rpl_msg		(int sock, eError rpl, const std::string& msg
												, const std::string& target) const;
	void				send_servers		(const std::string& prefix, const std::string& msg) const;
	void				send_servers		(const std::string& prefix, const std::string& msg, const int sock) const;
	void				send_servers		(const std::string& msg, const int sock) const;
	void				send_everyone		(const std::string& prefix, const std::string& msg) const;
	void				print_info			() const;
	int 				next_token			();
	int 				choose_sock			(AConnection* connection);
    eType           	connection_type     (int sock);
    eResult         	check_user_sender   (int sender_sock, User*& sender, const std::string& sender_name
                    	                        , User*& user, const std::string& user_nick);
    bool            	is_enough_args      (int sock, const std::string& command, int min_args_number);
    void            	send_channels       (int sock);
    User*				determine_user		(int sock);
	const std::string& 	get_user_modes		(const User& user);

	///	System messages
	std::string		sys_msg				(const std::string& emoji, const std::string& str) const;
	std::string		sys_msg				(const std::string& emoji, const std::string& str
											, const std::string& white_str) const;
	std::string		sys_msg				(const std::string& emoji, const std::string& str
											, const std::string& white_str, const std::string& ending) const;
	std::string		sys_msg				(const std::string& emoji, const std::string& str
											, const std::string& white_str, const std::string& str2
											, const std::string& ending) const;

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
    eResult         NOTICE              (const int sock);
	eResult			TIME				(const int sock);
	eResult			USERS				(const int sock);
	eResult			KILL				(const int sock);
	eResult			ADMIN				(const int sock);
	eResult			OPER				(const int sock);
	eResult			ERROR				(const int sock);
	eResult			RPL_256				(const int sock);
	eResult			RPL_257				(const int sock);
	eResult			RPL_258				(const int sock);
	eResult			RPL_259				(const int sock);
	eResult			RPL_421 			(const int sock);
	eResult			NAMES				(const int sock);
	eResult			LIST				(const int sock);
	eResult			INVITE				(const int sock);
	eResult			KICK				(const int sock);
	eResult			MOTD				(const int sock);
	eResult			MOTD_REPLIES		(const int sock);
	eResult			LUSERS				(const int sock);
	eResult			SQUIT				(const int sock);
    eResult         NJOIN               (const int sock);
	eResult			VERSION				(const int sock);
	eResult			CONNECT				(const int sock);
	eResult			STATS				(const int sock);
	eResult			LINKS				(const int sock);
	eResult			ISON				(const int sock);
	eResult			LUSERS_REPLIES		(const int sock);

	/// IRC commands utils
	void			admin_info			(const int sock, const std::string& receiver);
	void            send_channel    	(Channel *channel, std::string msg, std::string prefix);
    void            send_channel		(Channel *channel, std::string msg, std::string prefix, int sock);
    void            send_local_channel  (Channel *channel, std::string msg, std::string prefix, int sock);
	int             check_mode_channel	(const Channel* channel, const int sock, std::list<std::string>& arr_key, std::string& arr_channel);
	eResult			NICK_user			(User* const connection, const int sock, const std::string& new_nick);
	eResult			NICK_server			(const std::string& new_nick, int source_sock);
	std::string		createPASSmsg		(std::string password) const ;
	std::string		createSERVERmsg		(AConnection* server) const;
	std::string		createNICKmsg		(User* usr) const;
	void			send_motd			(const int sock);
	void			count_operators		(int& operators) const;
	void			count_global		(int& users, int& servers) const;
	void			count_local			(int& users, int& servers) const;
	void			send_lusers_replies	(const int sock) const;
	eResult 		send_bounce_reply	(int sock);
	eResult			resend_msg			(int sock);
	void 			connect_to_server	(const std::string& host_name, int port);
	void 			send_reg_info		(const std::string& pass);
	void 			send_servers_info	(int sock);
	void 			send_clients_info	(int sock);

	/// Error replies
	void			err_nosuchserver		(const int sock, const std::string& server) const;
	void			err_nosuchnick			(const int sock, const std::string& nick) const;
	void			err_nonicknamegiven		(const int sock) const;
	void			err_nicknameinuse		(const int sock, const std::string& nick) const;
	void			err_nickcollision		(const int sock, const std::string& nick) const;
	void			err_erroneusnickname	(const int sock, const std::string& nick) const;
	void			err_nosuchchannel		(const int sock, const std::string& channel) const;
	void			err_needmoreparams		(const int sock, const std::string& command) const;
	void			err_alreadyregistered	(const int sock) const;
	void			err_noorigin			(const int sock) const;
	void			err_norecipient			(const int sock, const std::string& command) const;
	void			err_notexttosend		(const int sock) const;
	void			err_notoplevel			(const int sock, const std::string& mask) const;
	void			err_wildtoplevel		(const int sock, const std::string& mask) const;
	void			err_cannotsendtochan	(const int sock, const std::string& channel) const;
	void			err_toomanytargets		(const int sock, const std::string& target) const;
	void			err_unknowncommand		(const int sock, const std::string& command) const;
	void			err_chanoprivsneeded	(const int sock, const std::string& channel) const;
	void			err_notochannel			(const int sock, const std::string& channel) const;
	void			err_keyset				(const int sock, const std::string& channel) const;
	void			err_unknownmode			(const int sock, const std::string& mode_char) const;
	void			err_usersdontmatch		(const int sock) const;
	void			err_umodeunknownflag	(const int sock) const;
	void			err_bannedfromchan		(const int sock, const std::string& channel) const;
	void			err_initeonlychan		(const int sock, const std::string& channel) const;
	void			err_channelisfull		(const int sock, const std::string& channel) const;
	void			err_toomanychannels		(const int sock, const std::string& channel) const;
	void			err_noprivileges		(const int sock) const;
	void			err_useronchannel		(const int sock, const std::string& user, const std::string& channel) const;
	void			err_usersdisabled		(const int sock) const;
	void			err_notregistered		(const int sock) const;
	void			err_passwdmismatch		(const int sock) const;
	void			err_yourebannedcreep	(const int sock) const;
	void 			err_nooperhost			(const int sock) const;
	void 			err_cantkillserver		(const int sock) const;

	/// Common Replies
	void			rpl_welcome				(const int sock) const;
	void			rpl_youreoper			(const int sock) const;
	void			rpl_time				(const int sock, const std::string& server, const std::string& local_time) const;
	void			rpl_away				(const int sock, const std::string& nick, const std::string& away_msg) const;
	void			rpl_channelmodeis		(const int sock, const std::string& mode, const std::string& mode_params) const;
	void			rpl_banlist				(const int sock, const std::string& channel, const std::string& ban_id) const;
	void			rpl_endofbanlist		(const int sock, const std::string& channel) const;
	void			rpl_info				(const int sock, const std::string& info) const;
	void			rpl_endofinfo			(const int sock) const;
	void			rpl_motdstart			(const int sock, const std::string& server) const;
	void			rpl_motd				(const int sock, const std::string& text) const;
	void			rpl_endofmotd			(const int sock) const;
	void			rpl_umodeis				(const int sock, const std::string& mode_string) const;
	void			rpl_topic				(const int sock, const std::string& channel, const std::string& topic) const;
	void			rpl_notopic				(const int sock, const std::string& channel) const;
	void			rpl_inviting			(const int sock, const std::string& channel, const std::string& nick) const;
	void			rpl_version				(const int sock, const std::string& target, const std::string& version, const std::string& debug_lvl
												, const std::string& server, const std::string& comments) const;
	void			rpl_adminme				(const int sock, const std::string& target, const std::string& server) const;
	void			rpl_adminloc1			(const int sock, const std::string& target, const std::string& info) const;
	void			rpl_adminloc2			(const int sock, const std::string& target, const std::string& info) const;
	void			rpl_adminmail			(const int sock, const std::string& target, const std::string& info) const;
	void			rpl_luserclient			(const int sock) const;
	void			rpl_luserop				(const int sock) const;
	void			rpl_luserchannels		(const int sock) const;
	void			rpl_luserunknown		(const int sock) const;
	void			rpl_luserme				(const int sock) const;
	void 			rpl_endofstats			(const int sock, const std::string& letter, const std::string &target) const;
	void 			rpl_statslinkinfo		(const int sock, const std::string &msg, const std::string &target);
	void 			rpl_statsuptime			(const int sock, const std::string &msg, const std::string &target);
	void 			rpl_links				(const int sock, const std::string &serv_name, int hopcount, const std::string &target);
	void 			rpl_endoflinks			(const int sock, const std::string &serv_name, const std::string &target);
	void 			rpl_ison				(const int sock, const std::string &nick);

	/// Unused constructors
	Irisha				() {};
	Irisha				(const Irisha& other) {};
	Irisha& operator=	(const Irisha& other) { return *this; };

public:
	explicit Irisha		(int port);
	Irisha				(int port, const std::string& password);
	Irisha				(const std::string& host_name, int network_port, const std::string& network_password
		   						, int port, const std::string& password);
	~Irisha				();


	/// ‼️ ⚠️ DEVELOPMENT UTILS (REMOVE OR COMMENT WHEN PROJECT IS READY) ⚠️ ‼️ //! TODO: DEV -> REMOVE ///
	enum ePrintMode
	{
		PM_LINE,
		PM_LIST,
		PM_ALL
	};

	void		print_cmd			(ePrintMode mode, const int sock) const;
	void		user_info			(const std::string& nick) const;
	void		print_user_list		() const;
	/// ‼️ ⚠️ END OF DEVELOPMENT UTILS ⚠️ ‼️ //! TODO: DEV -> REMOVE //////////////////////////////////////
};

#endif //FT_IRC_IRISHA_HPP
