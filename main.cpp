#include <string>

#include "Irisha.hpp"
#include "utils.hpp"
#include "parser.hpp"
void server_cycle(const std::string& host, int port_network, const std::string& password_network, int port, const std::string& password)
{
	Irisha server(2000);
	server.loop();
}

int	main(int argc, char *argv[])
{
    std::string host;
    int port_network;
    std::string password_network;
    int port;
    std::string password;

    parse_argv(argc, argv, host, port_network, password_network, port, password);
    std::cout << GREEN "HOST             = " CLR<< host << std::endl;
    std::cout << GREEN "PORT NETWORK     = " CLR<< port_network << std::endl;
    std::cout << GREEN "PASSWORD NETWORK = " CLR<< password_network << std::endl;
    std::cout << GREEN "PORT             = " CLR << port << std::endl;
    std::cout << GREEN "PASSWORD         = " CLR << password << std::endl;
	server_cycle(host, port_network, password_network, port, password);
	return 0;
}
