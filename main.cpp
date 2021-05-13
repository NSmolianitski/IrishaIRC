
#include "Irisha.hpp"

void server_cycle()
{
	//Irisha server("localhost", 6667, "hellothere", 2000, "psw");
	Irisha server(6667);
	server.loop();
}

int	main()
{
	server_cycle();
	return 0;
}
