
#include "Server.hpp"

void server_cycle()
{
	Server server(6667);
	server.loop();
}

int	main()
{
	server_cycle();
	return 0;
}
