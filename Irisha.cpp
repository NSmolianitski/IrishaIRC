//
// Created by Parfait Kentaurus on 5/6/21.
//

#include "Irisha.hpp"
#include "User.hpp"
#include "utils.hpp"

#include <thread>     //! TODO: REMOVE ///////////////////////////////////////////////////////////////////////////////////////////
#include <list>

#include <netdb.h>
#include <fcntl.h>

Irisha::Irisha(int port)
{
	init(port);
	launch();
}

Irisha::Irisha(int port, const std::string& password)
{
	init(port);
	password_ = password;
	launch();
}

Irisha::Irisha(const std::string& host_name, int network_port, const std::string& network_password,
			   int port, const std::string& password)
{
	struct sockaddr_in	server_address;

	init(port);
	launch();
	password_ = password;

	int speaker = socket(PF_INET, SOCK_STREAM, 0); //socket to connect with parent server
	if (speaker < 0) throw std::runtime_error("Socket opening error");

	FD_SET(speaker, &all_fds_);
	if (speaker > max_fd_)
		max_fd_ = speaker;

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(network_port);

	struct hostent	*host = gethostbyname(host_name.c_str());
	if (host == nullptr) throw std::runtime_error("No such host");

	bcopy(static_cast<char *>(host->h_addr)
			, reinterpret_cast<char *>(&server_address.sin_addr.s_addr)
			, host->h_length);

	int c = ::connect(speaker, reinterpret_cast<struct sockaddr *>(&server_address), sizeof(server_address));
	if (c < 0) throw std::runtime_error("Connection error");
	std::cout << "Connection established! " << "🔥" << "\n" << std::endl;

	//registration
	send_msg(speaker,createPASSmsg(network_password));
	send_msg(speaker, createSERVERmsg());
}

Irisha::~Irisha()
{
	close(listener_);
}

/**
 * @description	Applies config settings and checks for domain and password validity
 * @param		path: path to config
 */
void Irisha::apply_config(const std::string& path)
{
	check_config(path);
	domain_		= get_config_value(path, DOMAIN);
	password_	= get_config_value(path, PASS);
	int	dots	= 0;
	for (size_t i = 0; i < domain_.length(); ++i)
	{
		if (domain_[i] == '.')
		{
			if (domain_[i + 1] == '.')
				throw std::runtime_error("Config error: dots can't be near each other in domain name");
			++dots;
		}
	}
	if (dots < 2)
		throw std::runtime_error("Config error: server domain must contain at least two dots.");
	if (domain_.empty() || password_.empty())
		throw std::runtime_error("Config error: server domain and password must not be empty.");
}

/**
 * @description	Prints server information
 */
void Irisha::print_info()
{
	std::cout << BOLD UND "Current server configuration" << CLR "\n";
	std::cout << "domain: " << ITALIC PURPLE + domain_ << CLR "\n";
	std::cout << "password: " << ITALIC PURPLE + password_ << CLR << std::endl;
}

/**
 * @description	The launch() function binds socket and starts to listen
 */
void Irisha::launch()
{
	int option = 1;
	setsockopt(listener_, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)); // Flag for reusing port on restart

	int b = bind(listener_, reinterpret_cast<struct sockaddr *>(&address_), sizeof(address_));
	if (b == -1) throw std::runtime_error("Binding failed!");

	listen(listener_, 5);
	print_info();
	std::cout << BOLD WHITE "\n⭐ Server started. Waiting for the client connection. ⭐\n" CLR << std::endl;
}

void Irisha::init(int port)
{
	apply_config(CONFIG_PATH);
	listener_ = socket(PF_INET, SOCK_STREAM, 0);
	if (listener_ == -1) throw std::runtime_error("Socket creation failed!");

	FD_ZERO(&all_fds_);
	FD_SET(listener_, &all_fds_);

	max_fd_ = listener_;

	address_.sin_family = AF_INET;
	address_.sin_port = htons(port);
	address_.sin_addr.s_addr = INADDR_ANY;
}
/**
 * @description	Accepts one connection (server or client) and
 * 				sends greeting message
 * @return		connection_socket
 */
int Irisha::accept_connection()
{
	int client_socket = accept(listener_, nullptr, nullptr);
		if (client_socket == -1) throw std::runtime_error("Accepting failed");

	fcntl(client_socket, F_SETFL, O_NONBLOCK);

	FD_SET(client_socket, &all_fds_);
	if (client_socket > max_fd_)
		max_fd_ = client_socket;

	std::cout << ITALIC PURPLE "Client №" << client_socket << " connected! " << "⛄" CLR << std::endl;
	send_msg(client_socket, "✰ Welcome to Irisha server! ✰"); // Send greeting message
	return client_socket;
}

/**
 * @description	Sends a message to the client
 * @param		client_socket
 * @param		msg: message for the client
 */
void Irisha::send_msg(int client_socket, const std::string& msg) const
{
	std::string message = msg;
	message.append("\r\n");

	int n = send(client_socket, message.c_str(), message.length(), 0);
	if (n == -1) throw std::runtime_error("Send error");
}

/**
 * @description	Sends a message from input
 * @param		client_socket
 */
void Irisha::send_input_msg(int client_socket) const
{
	std::string message;
	getline(std::cin, message);
	if (message == "/exit" || message == "/EXIT")
		std::cout << ITALIC PURPLE "Server shutdown." CLR << std::endl;

	send_msg(client_socket, message);
	std::cout << ITALIC PURPLE "Message \"" + message + "\" was sent" CLR << std::endl;
}

/**
 * @description	Receives a message from the client
 * @param		client_socket
 * @return		message from the client
 */
std::string Irisha::get_msg(int client_socket)
{
	int read_bytes = recv(client_socket, &buff_, 510, 0);
	if (read_bytes < 0) throw std::runtime_error("Recv error in get_msg()");

	if (read_bytes == 0)
		handle_disconnection(client_socket);
	buff_[read_bytes] = '\0';
	return (buff_);
}

void sending_loop(const Irisha* server) //! TODO: REMOVE ///////////////////thread loop///////////////////////////////////////////////////////////////
{
	std::string	message;
	while (true)
	{
		getline(std::cin, message);
		message.append("\n");
		for (int i = 3; i < server->max_fd_ + 1; ++i)
		{
			if (FD_ISSET(i, &server->all_fds_) && i != server->listener_)
			{
				int send_bytes = send(i, message.c_str(), message.length(), 0);
				if (send_bytes < 0) throw std::runtime_error("Send error in send_msg()");
				std::cout << PURPLE ITALIC "Message sent to client №" << i << CLR << std::endl;
			}
		}
	}
} //! TODO: REMOVE //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @description	Launches the main server loop, which listens for
 * 				new clients, sends and receives messages
 */
void Irisha::loop()
{
	int			n;
	std::string	client_msg;
	std::list<Irisha::RegForm> reg_expect;	//not registered connections

	signal(SIGPIPE, SIG_IGN);
	std::thread	sender(sending_loop, this); //! TODO: REMOVE ////////////////////////////////////////////////////////////////////////////////////////////
	while (true)
	{
		read_fds_ = all_fds_;
		n = select(max_fd_ + 1, &read_fds_, nullptr, nullptr, nullptr);
		if (n == -1) throw std::runtime_error("Select error");

		for (int i = 3; i < max_fd_ + 1; ++i)
		{
			if (FD_ISSET(i, &read_fds_))
			{
				if (i == listener_)
				{
					accept_connection();
					reg_expect.push_back(RegForm(i));
				}
				else
				{
					client_msg = get_msg(i);
					if (!client_msg.empty())
						std::cout << "[" BLUE "Client №" << i << CLR "] " + client_msg << std::flush;
					Command cmd;
					std::list<RegForm>::iterator it = RegForm::expecting_registration(i, reg_expect);	//is this connection registered?
//					if (it != reg_expect.end())																//no, register it
//					{
//						int registered = register_connection(it, cmd);
//						if (registered)
//							reg_expect.erase(it);
//					}
					//else
					//	handle_command(cmd, i);								//yes, handle not registration command
				}
			}
		}
	}
	sender.detach(); //! TODO: REMOVE /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/**
 * @description	Closes client socket and removes if
 * 				from the all_fds_ member
 * @param		client_socket
 */
void Irisha::handle_disconnection(int client_socket)
{
	close(client_socket);
	FD_CLR(client_socket, &all_fds_);
	std::cout << ITALIC PURPLE "Client #" << client_socket << " closed connection. ☠" CLR << std::endl;
	FD_CLR(client_socket, &all_fds_);
	std::cout << ITALIC PURPLE "Client №" << client_socket << " closed connection. ☠" CLR << std::endl;
}

/// Commands

void Irisha::nick(const Command& cmd, const int socket)
{
	std::map<int, AConnection *>::iterator it = connections_.find(socket);
	if (it != connections_.end())	// Change nickname
	{
		User* user = dynamic_cast<User *>(it->second);
		if (user == nullptr) throw std::runtime_error("Object is not a User. Dynamic cast failed!");
	}
	else	// Add new user
	{
		if (cmd.arguments.empty() || !is_a_valid_nick(cmd.arguments[0]))
			return;
		AConnection* user = new User(socket, domain_, cmd.arguments[0]);
		connections_.insert(std::pair<int, AConnection*>(socket, user));
	}
}

/***************Creating message strings***************/

/**
 * @description	Returns PASS message string
 * @param		password - parent server password for connection
 * @return		PASS command string in this format: PASS <password> <version> <flags>
 */
std::string Irisha::createPASSmsg(std::string password)
{
	std::string msg = "PASS ";
	msg.append(password);
	msg.append(" 0210 Irisha| ");
	return msg;
}

/**
 * @description	Returns SERVER message string
 * @return		SERVER command string in this format: <servername> <info>
 */
std::string Irisha::createSERVERmsg()	///TODO: choose servername smarter
{
	std::string msg = "SERVER ";
	msg.append("irc2.example.net :Irisha server");
	return msg;
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
