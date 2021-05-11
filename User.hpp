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
	std::string	server_;
	int 		socket_;

	/// Unused constructors
	User() {};
	User(const User& other) {};
	User& operator= (const User& other) { return *this; };

public:
	User(int socket, const std::string server, int mod, const std::string& userName, const std::string& realName)
		: socket_(socket), server_(server), mod_(mod), userName_(userName), realName_(realName)
	{
		isOper_ = false;
		password_ = false;
	};

	~User() {};

	void		setNick			(const std::string& nick);
	void		setUserName		(const std::string& userName);
	void 		setRealName 	(const std::string& realName);
	void 		setPassword		(const std::string& password);
	void 		setMod			(int mod);
	void 		setIsOper		(bool isOper);
	void 		setNetwideId	(const std::string& netwideId);

	const std::string&	nick		() const;
	const std::string&	userName	() const;
	const std::string&	realName	() const;
	const std::string&	password	() const;
	int 				mod			() const;
	bool 				isOper		() const;
	const std::string&	netwideId	() const;
	const std::string&	server		() const;
	int					socket		() const;

};


#endif //FT_IRC_USER_HPP
