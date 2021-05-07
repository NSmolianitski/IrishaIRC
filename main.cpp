
#include "Server.hpp"

void server_cycle()
{
	Server server(2000);
	int a = server.accept_client();

	Server::Signal	signal = Server::S_NOSIGNAL;
	while (signal != Server::S_SHUTDOWN)
	{
		std::cout << "Enter message: ";
		signal = server.send_input_msg(a);
	}
}

int	main()
{

	return 0;
}
