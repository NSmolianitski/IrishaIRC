
#ifndef FT_IRC_USER_HPP
#define FT_IRC_USER_HPP

#include "AConnection.hpp"
#include "utils.hpp"
#include <string>

class User : public AConnection
{
private:
	std::string	nick_;
	std::string	username_;
	std::string realname_;
	std::string password_;
	int			mode_;		//! TODO: make mode number parsing
	std::string mode_str_;
	bool 		operator_;
	std::string netwideID_;
	std::string	server_;
	std::string host_;

	/// Unused constructors
	User() : AConnection(0, T_CLIENT,0, 0, 0) {};
	User(const User& other) : AConnection(0, T_CLIENT, 0, 0, 0) {};
	User& operator= (const User& other) { return *this; };

public:
	User(const int sock, const std::string& server, const std::string& nick); // Constructor for local user
	User(const int sock, const std::string& host, const int hopcount, const int source_sock, int token); // Constructor for external user
	~User();

	void	set_nick		(const std::string& nick);
	void	set_username	(const std::string& username);
	void	set_realname 	(const std::string& realname);
	void	set_password	(const std::string& password);
	void	set_mode		(const int mode);
	void	set_mode_str	(char mode);
	void	del_mode_str	(char mode);
	void	set_operator	(bool is_operator);
	void	set_netwideID	(const std::string& netwideID);

	const std::string&	nick		() const;
	const std::string&	username	() const;
	const std::string&	realname	() const;
	const std::string&	password	() const;
	int					mode		() const;
	const std::string&  mode_str    () const;
	bool 				is_operator	() const;
	const std::string&	netwideID	() const;
	const std::string&	server		() const;
	const std::string&	host		() const;


};


#endif //FT_IRC_USER_HPP
