//
// Created by Parfait Kentaurus on 5/10/21.
//

#ifndef FT_IRC_USER_HPP
#define FT_IRC_USER_HPP

#include <string>
#include "Irisha.hpp"

class User
{
private:
	//int			id_;
	std::string	nick_;
	std::string	username_;
	std::string real_name_;
	std::string password_;
	int 		mod_;
	bool 		operator_;
	std::string netwideID_;
	std::string	server_;
	int 		socket_;

	/// Unused constructors
	User() {};
	User(const User& other) {};
	User& operator= (const User& other) { return *this; };

public:
	User(int socket, const std::string& server, int mod, const std::string& username, const std::string& real_name)
		: socket_(socket), server_(server), mod_(mod), username_(username), real_name_(real_name)
	{
		operator_ = false;
		password_ = "";
	};

	~User() {};

	void	set_nick		(const std::string& nick);
	void	set_username	(const std::string& username);
	void	set_real_name 	(const std::string& real_name);
	void	set_password	(const std::string& password);
	void	set_mod			(int mod);
	void	set_operator	(bool is_operator);
	void	set_netwideID	(const std::string& netwideID);

	const std::string&	nick		() const;
	const std::string&	username	() const;
	const std::string&	real_name	() const;
	const std::string&	password	() const;
	int 				mod			() const;
	bool 				is_operator	() const;
	const std::string&	netwideID	() const;
	const std::string&	server		() const;
	int					socket		() const;

};


#endif //FT_IRC_USER_HPP
