#include "User.hpp"

void		User::setNick			(const std::string& nick)		{ nick_ = nick; }
void		User::setUserName		(const std::string& userName)	{ userName_ = userName; }
void 		User::setRealName 	(const std::string& realName)	{ realName_ = realName; }
void 		User::setPassword		(const std::string& password)	{ password_ = password; }
void 		User::setMod			(int mod)						{ mod_ = mod; }
void 		User::setIsOper		(bool isOper)					{ isOper_ = isOper; }
void 		User::setNetwideId	(const std::string& netwideId)	{ netwideId_ = netwideId; }

const std::string&	User::nick		() const { return nick_; }
const std::string&	User::userName	() const { return userName_; }
const std::string&	User::realName	() const { return realName_; }
const std::string&	User::password	() const { return password_; }
int 				User::mod		() const { return mod_; }
bool 				User::isOper	() const { return isOper_; }
const std::string&	User::netwideId	() const { return netwideId_; }
const std::string&	User::server	() const { return server_; }
int					User::socket	() const {return socket_; }