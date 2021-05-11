#include "User.hpp"

void	User::set_nick		(const std::string& nick)		{ nick_ = nick; }
void	User::set_username	(const std::string& username)	{ username_ = username; }
void	User::set_real_name (const std::string& real_name)	{ real_name_ = real_name; }
void	User::set_password	(const std::string& password)	{ password_ = password; }
void	User::set_mod		(int mod)						{ mod_ = mod; }
void	User::set_operator	(bool is_operator)				{ operator_ = is_operator; }
void	User::set_netwideID	(const std::string& netwideID)	{ netwideID_ = netwideID; }

const std::string&	User::nick			() const { return nick_; }
const std::string&	User::username		() const { return username_; }
const std::string&	User::real_name		() const { return real_name_; }
const std::string&	User::password		() const { return password_; }
int 				User::mod			() const { return mod_; }
bool 				User::is_operator	() const { return operator_; }
const std::string&	User::netwideID		() const { return netwideID_; }
const std::string&	User::server		() const { return server_; }
int					User::socket		() const { return socket_; }