//
// Created by Parfait Kentaurus on 5/10/21.
//

#ifndef FT_IRC_USER_HPP
#define FT_IRC_USER_HPP

#include <string>
#include "Server.hpp"

class User
{
private:
	//int			id_;
	std::string	nick_;
	std::string	userName_;
	std::string realName_;
	std::string password_;
	int 		mod_;
	bool 		isOper_;
	std::string netwideId_;
	Server&		server_;
	int 		socket_;

	/// Unused constructors
	User() {};
	User(const User& other) {};
	User& operator= (const User& other) { return *this; };

public:
	User(int socket, const Server& server, int mod, const std::string& userName, const std::string& realName)
		: socket_(socket), server_(server), mod_(mod), userName_(userName), realName_(realName)
	{
		isOper_ = false;
		password_ = false;
	};

	~User() {};

	void		setNick			(const std::string& nick)		{ nick_ = nick; }
	void		setUserName		(const std::string& userName)	{ userName_ = userName; }
	void 		setRealName 	(const std::string& realName)	{ realName_ = realName; }
	void 		setPassword		(const std::string& password)	{ password_ = password; }
	void 		setMod			(int mod)						{ mod_ = mod; }
	void 		setIsOper		(bool isOper)					{ isOper_ = isOper; }
	void 		setNetwideId	(const std::string& netWideId)	{ netWideId_ = netWideId; }

	const std::string&	nick		() const { return nick_; }
	const std::string&	userName	() const { return userName_; }
	const std::string&	realName	() const { return realName_; }
	const std::string&	password	() const { return password_; }
	int 				mod			() const { return mod_; }
	bool 				isOper		() const { return isOper_; }
	const std::string&	netwideId	() const { return netwideId_; }
	const Server&		server		() const { return server_; }
	int					socket		() const {return socket_; }

};


#endif //FT_IRC_USER_HPP
