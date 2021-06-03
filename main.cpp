
#include "Irisha.hpp"
#include "utils.hpp"
#include "parser.hpp"

#include <string>

void server_cycle(const std::string& host, int port_network, const std::string& password_network, int port, const std::string& password)
{
	try
	{
		if (!host.empty() && port_network && !password_network.empty() && port && !password.empty())
			Irisha server(host, port_network, password_network, port, password);
		else
			Irisha server(port, password);
	}
	catch (std::exception& e)
	{
		std::cerr << RED BOLD E_SCULL " ALARM! " << e.what() << " " E_SCULL CLR << std::endl;
		exit(1);
	}
}

int	main(int argc, char *argv[])
{
    std::string	host;
    int			port_network;
    std::string	password_network;
    int			port;
    std::string	password;
	try
	{
    	parse_argv(argc, argv, host, port_network, password_network, port, password);
	}
	catch (std::exception& e)
	{
		std::cerr << RED BOLD E_SCULL " ALARM! " << e.what() << " " E_SCULL CLR << std::endl;
		exit(1);
	}
    std::cout << GREEN "HOST             = " CLR<< host << std::endl;
    std::cout << GREEN "PORT NETWORK     = " CLR<< port_network << std::endl;
    std::cout << GREEN "PASSWORD NETWORK = " CLR<< password_network << std::endl;
    std::cout << GREEN "PORT             = " CLR << port << std::endl;
    std::cout << GREEN "PASSWORD         = " CLR << password << std::endl;
	server_cycle(host, port_network, password_network, port, password);
	return 0;
}
