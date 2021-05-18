//
// Created by Parfait Kentaurus on 5/12/21.
//

#ifndef FT_IRC_ACONNECTION_HPP
#define FT_IRC_ACONNECTION_HPP

#include <string>

enum eType
{
	T_SERVER,
	T_CLIENT
};

class AConnection
{
private:
	int			socket_;
	eType		type_;
	int			hopcount_;
	std::string	buff_;

	/// Unused constructors
	AConnection();
public:
	AConnection(int socket, eType type, int hopcount);
	virtual ~AConnection() = 0;

	int 			socket	() const;
	eType 			type	() const;
	int 			hopcount() const;
	std::string&	buff	();
};


#endif //FT_IRC_ACONNECTION_HPP
