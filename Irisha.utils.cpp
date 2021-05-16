//
// Created by Parfait Kentaurus on 5/14/21.
//

#include "Irisha.hpp"
#include "utils.hpp"

/**
 * @description	Receives a message from socket
 * @param		socket: sender socket
 * @return		message received from socket
 */
std::string Irisha::get_msg(int sock)
{
	int read_bytes = recv(sock, &buff_, 510, 0);
	if (read_bytes < 0) throw std::runtime_error("Recv error in get_msg()");

	if (read_bytes == 0)
		handle_disconnection(sock);
	buff_[read_bytes] = '\0';
	return (buff_);
}

/**
 * @description	Prints server information
 */
void Irisha::print_info() const
{
	std::cout << BOLD UND "Current server configuration" << CLR "\n";
	std::cout << "domain: " << ITALIC PURPLE + domain_ << CLR "\n";
	std::cout << "password: " << ITALIC PURPLE + password_ << CLR << std::endl;
}

/**
 * @description	Sends a message to socket
 * @param		sock: receiver socket
 * @param		msg: message
 */
void Irisha::send_msg(int sock, const std::string& prefix, const std::string& msg) const
{
	std::string message;
	if (!prefix.empty())
		message = ":" + prefix + " " + msg;
	else
		message = msg;
	message.append("\r\n");

//	std::cout << message << std::endl;
	ssize_t n = send(sock, message.c_str(), message.length(), 0);
	if (n == -1) throw std::runtime_error("Send error");
}

/**
 * @description	Sends a message from input
 * @param		sock: receiver socket
 */
void Irisha::send_input_msg(int sock) const
{
	std::string input;
	getline(std::cin, input);
	if (input == "/exit" || input == "/EXIT")
	{
		std::cout << ITALIC PURPLE "Server shutdown." CLR << std::endl;
		exit(0);
	}

	std::string message = ":" + domain_ + " " + input;
	message.append("\r\n");

	ssize_t n = send(sock, message.c_str(), message.length(), 0);
	if (n == -1) throw std::runtime_error("Send error");
}

/**
 * @description	Calls IRC command if it exists
 * @param		sock
 */
void Irisha::handle_command(int sock)
{
	std::map<std::string, func>::const_iterator it = commands_.find(cmd_.command);
	if (it != commands_.end())	// Execute command
		((*this).*it->second)(sock);
	else
		send_msg(sock, domain_, ":No such command, my friend"); //! TODO: change to error reply (421, "<command> :Unknown command")
}


void sending_loop(const Irisha* server) //! TODO: REMOVE ///////////////////thread loop///////////////////////////////////////////////////////////////
{
	std::string serv_prefix = ":" + server->domain_ + " ";
	std::string input, message;
	while (true)
	{
		getline(std::cin, input);
		input.append("\r\n");
		if (input[0] == '*' && input[1] == ' ')
			input = input.replace(0, 2, serv_prefix);
		message.append(input);
		for (int i = 3; i < server->max_fd_ + 1; ++i)
		{
			if (FD_ISSET(i, &server->all_fds_) && i != server->listener_)
			{
				int send_bytes = send(i, message.c_str(), message.length(), 0);
				if (send_bytes < 0) throw std::runtime_error("Send error in send_msg()");
				message.pop_back();
				message.pop_back();
				std::cout << PURPLE ITALIC "Message \"" + message + "\" sent to socket №" << i << CLR << std::endl;
			}
		}
		message.clear();
	}
}
//! TODO: REMOVE //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////