//
// Created by Parfait Kentaurus on 5/6/21.
//

#include "Server.hpp"
#include "Command.hpp"
#include "utils.hpp"

#include <fcntl.h>
#include <thread>     //! TODO: REMOVE ///////////////////////////////////////////////////////////////////////////////////////////

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
	std::cout << BOLD WHITE "⭐ Server started. Waiting for the client connection. ⭐\n" CLR << std::endl;
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

	std::cout << ITALIC PURPLE "Client №" << client_socket << " connected! " << "⛄" CLR << std::endl;
	send_msg(client_socket, ":serv1.irisha.net ✰ Welcome to Irisha server! ✰"); // Send greeting message
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
	std::string message = msg;
	message.append("\r\n");

	int n = send(client_socket, message.c_str(), message.length(), 0);
	if (n == -1) throw std::runtime_error("Send error");
}

/**
 * @description	The send_input_msg() function sends a message from input
 *
 * @param		client_socket
 */
void Server::send_input_msg(int client_socket) const
{
	std::string message;
	getline(std::cin, message);
	if (message == "/exit" || message == "/EXIT")
		std::cout << ITALIC PURPLE "Server shutdown." CLR << std::endl;

	send_msg(client_socket, message);
	std::cout << ITALIC PURPLE "Message \"" + message + "\" was sent" CLR << std::endl;
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

void sending_loop(const Server* server) //! TODO: REMOVE //////////////////////////////////////////////////////////////////////////
{
	std::string	message;
	while (true)
	{
		getline(std::cin, message);
		message.append("\n");
		for (int i = 3; i < server->max_fd_ + 1; ++i)
		{
			if (FD_ISSET(i, &server->client_fds_) && i != server->listener_)
			{
				int send_bytes = send(i, message.c_str(), message.length(), 0);
				if (send_bytes < 0) throw std::runtime_error("Send error in send_msg()");
				std::cout << PURPLE ITALIC "Message sent to client №" << i << CLR << std::endl;
			}
		}
	}
} //! TODO: REMOVE //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @description	The loop() function launches the main server loop, which listens for
 * 				new clients, sends and receives messages
 */
void Server::loop()
{
	int			n;
	std::string	client_msg;

	signal(SIGPIPE, SIG_IGN);
	std::thread	sender(sending_loop, this); //! TODO: REMOVE ////////////////////////////////////////////////////////////////////////////////////////////
	while (true)
	{
		read_fds_ = client_fds_;
		n = select(max_fd_ + 1, &read_fds_, nullptr, nullptr, nullptr);
		if (n == -1) throw std::runtime_error("Select error");

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
						std::cout << "[" BLUE "Client №" << i << CLR "] " + client_msg << std::flush;
				}
			}
		}
	}
	sender.detach(); //! TODO: REMOVE /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/**
 * @description	The handle_disconnection() function closes client socket and removes if
 * 				from the client_fds_ member
 *
 * @param		client_socket
 */
void Server::handle_disconnection(int client_socket)
{
	close(client_socket);
	FD_CLR(client_socket, &client_fds_);
	std::cout << ITALIC PURPLE "Client №" << client_socket << " closed connection. ☠" CLR << std::endl;
}

/// Commands

void Server::nick()
{

}

/*
 * COMMANDS:
 *
 * :localhost 001 Guest52 ⭐ Welcome to Irisha server! ⭐
 * :localhost PING Guest52
 * :localhost PONG
 * :localhost 371 Guest52 :info
 *
 * :localhost 332 Guest52 #shell :topic
 * :localhost 353 Guest52 = #shell :@doc amy Guest52
 * :localhost PRIVMSG #shell :topic
 *
 * :amy PRIVMSG #channel :message
 * :amy PRIVMSG Guest52 Guest50 Guest51 #shell :message
 */
