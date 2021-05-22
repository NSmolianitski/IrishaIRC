
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
	int 		token_;
	int 		source_socket_;
	std::string	buff_;
	time_t		last_msg_time_;

	/// Unused constructors
	AConnection();
public:
	AConnection(int socket, eType type, int hopcount, int source_socket, int token);
	virtual ~AConnection() = 0;

	int 			socket				() const;
	eType 			type				() const;
	int 			hopcount			() const;
	std::string&	buff				();
	void			update_time			();
	double			last_msg_time		() const;
	int 			token				() const;
};


#endif //FT_IRC_ACONNECTION_HPP
