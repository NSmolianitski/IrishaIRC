
#include "Server.hpp"

#include <set>
#include <fcntl.h>

void server_cycle()
{
	Server server(2000);
//	int a = server.accept_client();

//	std::cout << server.get_msg(a);
//	server.send_msg(a, ":localhost 001 Guest52 : Welcome to the Internet Relay Network Guest52");
//	server.send_msg(a, ":localhost Guest52 PRIVMSG Guest52 : Hello");

	server.loop();
}

int	main()
{
	server_cycle();
	return 0;
}
