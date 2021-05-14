#include "Server.hpp"

void server_cycle()
{
    Server server(2000);
    server.loop();
}

int	main()
{
    server_cycle();
    return 0;
}