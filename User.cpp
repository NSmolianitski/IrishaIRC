
#include "User.hpp"


/**
 * @description	Default User constructor
 * @param		sock
 * @param		username
 * @param		server
 * @param		mode
 * @param		real_name
 */
User::User(const int sock, const std::string& server, const std::string& nick)
	: AConnection(sock, T_CLIENT, 0, sock), server_(server), nick_(nick), operator_(false)
{
	host_ = get_sock_host(sock);
}

User::User(const int sock, const std::string& server, const int hopcount, const int source_sock)
	: AConnection(sock, T_CLIENT, hopcount, source_sock), server_(server), operator_(false)
{

}

User::~User() {}

void	User::set_nick		(const std::string& nick)		{ nick_ = nick; }
void	User::set_username	(const std::string& username)	{ username_ = username; }
void	User::set_realname (const std::string& realname)	{ realname_ = realname; }
void	User::set_password	(const std::string& password)	{ password_ = password; }
void	User::set_mode		(const int mode)				{ mode_ = mode; }
void	User::set_operator	(bool is_operator)				{ operator_ = is_operator; }
void	User::set_netwideID	(const std::string& netwideID)	{ netwideID_ = netwideID; }

const std::string&	User::nick			() const { return nick_; }
const std::string&	User::username		() const { return username_; }
const std::string&	User::realname		() const { return realname_; }
const std::string&	User::password		() const { return password_; }
int					User::mode			() const { return mode_; }
bool 				User::is_operator	() const { return operator_; }
const std::string&	User::netwideID		() const { return netwideID_; }
const std::string&	User::server		() const { return server_; }
