//
// Created by Parfait Kentaurus on 5/10/21.
//

#ifndef FT_IRC_USER_HPP
#define FT_IRC_USER_HPP

#include <string>

class User
{
private:
	int			id_;
	std::string	nick_;
	std::string	name_;

	/// Unused constructors
	User() {};
	User(const User& other) {};
	User& operator= (const User& other) { return *this; };

public:
	User(int id, const std::string& nick) : id_(id), nick_(nick) {};
	~User() {};

	void		set_id		(int id) { id_ = id; }
	void		set_nick	(const std::string& nick) { nick_ = nick; }
	void		set_name	(const std::string& name) { name_ = name; }

	int			id			() const { return id_; }
	std::string nick		() const { return nick_; }
	std::string name		() const { return name_; }

};


#endif //FT_IRC_USER_HPP
