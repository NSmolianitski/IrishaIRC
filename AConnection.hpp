//
// Created by Parfait Kentaurus on 5/12/21.
//

#ifndef FT_IRC_ACONNECTION_HPP
#define FT_IRC_ACONNECTION_HPP

enum Type
{
	T_SERVER,
	T_CLIENT
};

class AConnection
{
private:
	int		socket_;
	Type	type_;
	int		hopcount_;

	/// Unused constructors
	AConnection();
public:
	AConnection(int socket, Type type, int hopcount);
	virtual ~AConnection() = 0;

	int socket	() const;
	int type	() const;
	int hopcount() const;
};


#endif //FT_IRC_ACONNECTION_HPP
