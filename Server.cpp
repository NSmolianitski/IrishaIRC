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
int Server::accept_client() const
{
	int client_socket = accept(listener_, nullptr, nullptr);
	fcntl(client_socket, F_SETFL, O_NONBLOCK);

	std::cout << "Client connected! ⛄" << std::endl;
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
	buff_[read_bytes] = '\0';
	return (buff_);
}
