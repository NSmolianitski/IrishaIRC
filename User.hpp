//
// Created by Parfait Kentaurus on 5/10/21.
//

#ifndef FT_IRC_USER_HPP
#define FT_IRC_USER_HPP

#include "AConnection.hpp"
#include "Irisha.hpp"

#include <string>

class User : public AConnection
{
private:
	//int		id_;
	std::string	nick_;
	std::string	username_;
	std::string real_name_;
	std::string password_;
	int 		mod_;
	bool 		operator_;
	std::string netwideID_;
	std::string	server_;

	/// Unused constructors
	User() : AConnection(0, T_CLIENT) {};
	User(const User& other) : AConnection(0, T_CLIENT) {};
	User& operator= (const User& other) { return *this; };

public:
	User(int socket, const std::string& server, int mod, const std::string& username, const std::string& real_name);
	~User();

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

};


#endif //FT_IRC_USER_HPP
