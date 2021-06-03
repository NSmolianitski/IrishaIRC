
#ifndef FT_IRC_USER_HPP
#define FT_IRC_USER_HPP

#include "AConnection.hpp"
#include "utils.hpp"
#include <string>
#include <vector>

class User : public AConnection
{
private:
	std::string	nick_;
	std::string	username_;
	std::string realname_;
	std::string password_;
	int			mode_;
	std::string mode_str_;
	bool 		operator_;
	std::string netwideID_;
	std::string	server_;
	std::string host_;
	std::vector<std::string> channels_;

	/// Unused constructors
	User() : AConnection(0, T_CLIENT,0, 0, 0) {};
	User(const User& other) : AConnection(0, T_CLIENT, 0, 0, 0) { std::cout << other.nick(); };
	User& operator= (const User& other) { std::cout << other.nick(); return *this; };

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
	void 	set_mode_str	(const std::string& mode_str);
	void	del_mode_str	(char mode);
	void	set_operator	(bool is_operator);
	void	set_netwideID	(const std::string& netwideID);
    void    set_channel     (const std::string& channel);
    void    del_channel     (const std::string& channel);

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
    std::vector<std::string> channels() const;
};


#endif //FT_IRC_USER_HPP
