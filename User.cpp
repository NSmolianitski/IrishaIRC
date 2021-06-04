
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
		: AConnection(sock, T_CLIENT, 0, sock, 1), nick_(nick), operator_(false), server_(server)
{
	host_ = std::string(get_sock_host(sock));

}

User::User(const int sock, const std::string& host, const int hopcount, const int source_sock, int token)
		: AConnection(sock, T_CLIENT, hopcount, source_sock, token), operator_(false), host_(host)
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
void    User::set_mode_str  (char mode)                     { mode_str_.push_back(mode ); }
void    User::set_mode_str(const std::string &mode_str)     { mode_str_ = mode_str; }
void    User::del_mode_str  (char mode)                     { mode_str_.erase(mode_str_.find(mode)); }

const std::string&	User::nick			() const { return nick_; }
const std::string&	User::username		() const { return username_; }
const std::string&	User::realname		() const { return realname_; }
const std::string&	User::password		() const { return password_; }
int					User::mode			() const { return mode_; }
bool 				User::is_operator	() const { return operator_; }
const std::string&	User::netwideID		() const { return netwideID_; }
const std::string&	User::server		() const { return server_; }
const std::string&	User::host			() const { return host_; }
const std::string&	User::mode_str		() const {return mode_str_; }

void User::set_channel(const std::string &channel) {
	std::vector<std::string>::iterator itr = channels_.begin();

	while (itr != channels_.end())
	{
		if (*itr == channel)
			return;
		itr++;
	}
	channels_.push_back(channel);
}

void User::del_channel(const std::string &channel) {
	std::vector<std::string>::iterator itr = channels_.begin();

	while (itr != channels_.end())
	{
		if (*itr == channel){
			channels_.erase(itr);
			return;
		}
		itr++;
	}
}

std::vector<std::string>& User::channels() {
	return channels_;
}
