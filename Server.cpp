//
// Created by Parfait Kentaurus on 5/6/21.
//

#include "Server.hpp"

#include <fcntl.h>

Server::Server(int port)
{
	listener_ = socket(PF_INET, SOCK_STREAM, 0);
	if (listener_ == -1) throw std::runtime_error("Socket creation failed!");
	//! TODO: socket error check

	FD_ZERO(&client_fds_);
	FD_SET(listener_, &client_fds_);

	max_fd_ = listener_;

	address_.sin_family = AF_INET;
	address_.sin_port = htons(port);
	address_.sin_addr.s_addr = INADDR_ANY;
	launch();
}

Server::~Server()
{
	close(listener_);
}

/**
 * @description	The launch() function binds socket and starts to listen
 */
void Server::launch()
{
	int option = 1;
	setsockopt(listener_, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)); // Flag for reusing port on restart

	int b = bind(listener_, reinterpret_cast<struct sockaddr *>(&address_), sizeof(address_));
	if (b == -1) throw std::runtime_error("Binding failed!");
	//! TODO: binding error check

	listen(listener_, 5);
	std::cout << "Server started. Waiting for the client connection." << std::endl;
}

/**
 * @description	The accept_client() function accepts one client and
 * 				sends greeting message
 *
 * @return		client_socket
 */
int Server::accept_client()
{
	int client_socket = accept(listener_, nullptr, nullptr);
		if (client_socket == -1) throw std::runtime_error("Accepting failed");

	fcntl(client_socket, F_SETFL, O_NONBLOCK);

	FD_SET(client_socket, &client_fds_);
	if (client_socket > max_fd_)
		max_fd_ = client_socket;

	std::cout << "Client #" << client_socket << " connected! ⛄" << std::endl;
	send_msg(client_socket, "✰ Welcome to the Irishka's server! ✰"); // Send greeting message
	return client_socket;
}

/**
 * @description	The send_msg() function sends a message to the client
 *
 * @param		client_socket
 * @param		msg: message for the client
 */
void Server::send_msg(int client_socket, const std::string& msg) const
{
	send(client_socket, msg.c_str(), msg.length(), 0);
}

/**
 * @description	The send_input_msg() function sends a message from input
 *
 * @param		client_socket
 */
Server::Signal Server::send_input_msg(int client_socket) const
{
	std::string message;
	getline(std::cin, message);
	if (message == "/exit" || message == "/EXIT")
	{
		std::cout << "Server shutdown." << std::endl;
		return S_SHUTDOWN;
	}

	send_msg(client_socket, message);
	std::cout << "Message \"" + message + "\" was sent" << std::endl;
	return S_MSG_SENT;
}

/**
 * @description	The get_msg() function receives a message from the client
 *
 * @param		client_socket
 * @return		message from the client
 */
std::string Server::get_msg(int client_socket)
{
	int read_bytes = recv(client_socket, &buff_, 510, 0);
	if (read_bytes < 0) throw std::runtime_error("Recv error in get_msg()");

	if (read_bytes == 0)
		handle_disconnection(client_socket);
	buff_[read_bytes] = '\0';
	return (buff_);
}

/**
 * @description	The loop() function launches the main server loop, which listens for
 * 				new clients, sends and receives messages
 */
void Server::loop()
{
	int			n;
	std::string	client_msg;

	while (true)
	{
		read_fds_ = client_fds_;
		n = select(max_fd_ + 1, &read_fds_, nullptr, nullptr, nullptr);
		if (n == -1) throw std::runtime_error("select error");

		for (int i = 3; i < max_fd_ + 1; ++i)
		{
			if (FD_ISSET(i, &read_fds_))
			{
				if (i == listener_)
					accept_client();
				else
				{
					client_msg = get_msg(i);
					if (!client_msg.empty())
						std::cout << "[Client#" << i << "] " + client_msg << std::endl;
				}
			}
		}
	}
}

int Server::handle_disconnection(int client_socket)
{
	std::cout << "Client #" << client_socket << " closed connection." << std::endl;
	close(client_socket);
	FD_CLR(client_socket, &client_fds_);
}
