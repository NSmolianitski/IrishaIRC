
#include "Irisha.hpp"

void server_cycle()
{
	Irisha server(6667);
	server.loop();
}

int	main()
{
	server_cycle();
	return 0;
}
