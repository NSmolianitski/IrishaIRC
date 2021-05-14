
#include "Irisha.hpp"
#include "utils.hpp"

void server_cycle()
{
	try
	{
//		Irisha server("localhost", 6667, "secret", 2000, "psw");
		Irisha server(6667);
		server.loop();
	}
	catch (std::exception& e)
	{
		std::cerr << RED BOLD "☠ " << "ALARM! " << e.what() << " ☠" CLR << std::endl;
		exit(1);
	}
}

int	main()
{
	server_cycle();
	return 0;
}
