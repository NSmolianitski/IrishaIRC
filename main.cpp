
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

	std::set<int> clients;

	fd_set readset;
	FD_ZERO(&readset);
	FD_SET(server.get_listener(), &readset);

	timeval timeout = { 15, 0 };

	int mx = std::max(server.get_listener(), *max_element(clients.begin(), clients.end()));
	if(select(mx + 1, &readset, nullptr, nullptr, &timeout) <= 0)
	{
		perror("select");
		exit(3);
	}
	if (FD_ISSET(server.get_listener(), &readset))
	{
		// Поступил новый запрос на соединение, используем accept
		int sock = accept(server.get_listener(), nullptr, nullptr);
		if (sock < 0) throw std::runtime_error("Connection accept error");

		fcntl(sock, F_SETFL, O_NONBLOCK);
		clients.insert(sock);
	}

	char buf[512];
	for (std::set<int>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (FD_ISSET(*it, &readset))
		{
			// Поступили данные от клиента, читаем их
			int bytes_read = recv(*it, buf, 512, 0);
			buf[bytes_read] = '\0';
			std::cout << buf << std::endl;

			if (bytes_read <= 0)
			{
				// Соединение разорвано, удаляем сокет из множества
				close(*it);
				clients.erase(*it);
				continue;
			}
		}
	}

	Server::Signal	signal = Server::S_NOSIGNAL;
	while (signal != Server::S_SHUTDOWN)
	{
		std::cout << "Enter message: ";
		signal = server.send_input_msg(*clients.begin());
	}
}

int	main()
{
	server_cycle();
	return 0;
}
