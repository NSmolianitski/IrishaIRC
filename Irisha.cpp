//
// Created by Parfait Kentaurus on 5/6/21.
//

#include "Irisha.hpp"
#include "User.hpp"
#include "utils.hpp"
#include "Server.hpp"
#include "parser.hpp"

#include <thread>     //! TODO: REMOVE ///////////////////////////////////////////////////////////////////////////////////////////
#include <list>

#include <netdb.h>
#include <fcntl.h>

Irisha::Irisha(int port)
{
	init(port);
	launch();
	print_info();
	std::cout << BOLD WHITE "\n⭐ Server started. Waiting for the client connection. ⭐\n" CLR << std::endl;
}

Irisha::Irisha(int port, const std::string& password)
{
	init(port);
	password_ = password;
	launch();
	print_info();
	std::cout << BOLD WHITE "\n⭐ Server started. Waiting for the client connection. ⭐\n" CLR << std::endl;
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
	print_info();
	std::cout << BOLD WHITE "\n⭐ Server started. Waiting for the client connection. ⭐\n" CLR << std::endl;

	//registration
	send_msg(speaker, NO_PREFIX, createPASSmsg(network_password));
	send_msg(speaker, NO_PREFIX, createSERVERmsg());
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
	//password_	= get_config_value(path, PASS);
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
	if (domain_.empty())
		throw std::runtime_error("Config error: server domain and password must not be empty.");
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
 * @return		connection socket
 */
int Irisha::accept_connection()
{
	int sock = accept(listener_, nullptr, nullptr);
		if (sock == -1) throw std::runtime_error("Accepting failed");

	fcntl(sock, F_SETFL, O_NONBLOCK);

	FD_SET(sock, &all_fds_);
	if (sock > max_fd_)
		max_fd_ = sock;

	std::cout << ITALIC PURPLE "Client №" << sock << " connected! " << "⛄" CLR << std::endl;
	send_msg(sock, domain_, "✰ Welcome to Irisha server! ✰"); // Send greeting message
	return sock;
}

/**
 * @description	Launches the main server loop, which listens for
 * 				new clients, sends and receives messages
 */
void Irisha::loop()
{
	int			n;
	std::string	client_msg;
	std::list<Irisha::RegForm> reg_expect;	//not registered connections
    std::deque<std::string> arr_msg;		// array messages, not /r/n

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
					int connection_fd = accept_connection();
					reg_expect.push_back(RegForm(connection_fd));
				}
				else
				{
					client_msg = get_msg(i);
                    parse_arr_msg(arr_msg, client_msg);
                    while (!arr_msg.empty())
                    {
                        parse_msg(arr_msg[0], cmd_);
						std::cout << "[" BLUE "Client №" << i << CLR "] " + arr_msg[0] << std::endl;
                        arr_msg.pop_front();
                    }
					Command cmd;
					std::list<RegForm>::iterator it = expecting_registration(i, reg_expect);	//is this connection waiting registration?
					if (it != reg_expect.end())													//yes, register it
					{
						if (it->pass_received_ == false) ///delete this
						{
							cmd.command = "PASS";
							cmd.arguments.push_back("hellothere");
						}
						else								///and this
						{
							cmd.command = "SERVER";
							cmd.arguments.push_back("monkey.irisha.net");
						}
						int registered = register_connection(it, cmd);
						if (registered == 0)
							reg_expect.erase(it);
					}
					//else
						handle_command(i);								//yes, handle not registration command
				}
			}
		}
	}
	sender.detach(); //! TODO: REMOVE ////<======///////////////////////////////////////////////////////////////////////////////
}

/**
 * @description	Closes client socket and removes if
 * 				from the all_fds_ member
 * @param		client_socket
 */
void Irisha::handle_disconnection(int sock)
{
	close(sock);
	FD_CLR(sock, &all_fds_);
	std::cout << ITALIC PURPLE "Client #" << sock << " closed connection. ☠" CLR << std::endl;
	FD_CLR(sock, &all_fds_);
	std::cout << ITALIC PURPLE "Client №" << sock << " closed connection. ☠" CLR << std::endl;
}

/// Commands+

/**
 * Handling PASS message
 * @param fd - socket
 * @param cmd - parsed message
 * @return 0 if password correct, else 1
 */
int Irisha::PASS(int fd, const Command& cmd)
{
	if (cmd.arguments.empty())
		return 1;
	if (password_ == cmd.arguments[0])
		return 0;
	else
		return 1;
}

/**
 *Handling SERVER message
 * @param fd - socket
 * @param cmd - parsed message
 * @return 0 if registration successfully, else 1
 */
int Irisha::SERVER(int fd, const Command &cmd)
{
	//place for your validator
	int hopcount = 1;

	if (cmd.prefix.empty())
		hopcount = 1;
	//else
		//take hopcount from argiments
	AConnection* server = new Server(cmd.arguments[0], fd, hopcount);
	connections_.insert(std::pair<int, AConnection*>(fd, server));
	return 0;
}

///Commands-

/**
 *
 * @param i - socket to identify connection
 * @param reg_expect - list of not registered connections
 * @return	node iterator, if socket in list, else end iterator
 */
std::list<Irisha::RegForm>::iterator Irisha::expecting_registration(int i, std::list<RegForm>& reg_expect)
{
	std::list<RegForm>::iterator it;
	for (it = reg_expect.begin(); it != reg_expect.end(); it++)
	{
		if (it->fd_ == i)
			return it;
	}
	return (reg_expect.end());
}


///TODO: handle client connection
int			Irisha::register_connection	(std::list<Irisha::RegForm>::iterator rf, Command& cmd)
{
	if (rf->pass_received_ == false)
	{
		if (cmd.command == "PASS" && (PASS(rf->fd_, cmd) == 0))
			rf->pass_received_ = true;
		return 1;
	}
	else
	{
		if (cmd.command == "SERVER" && (SERVER(rf->fd_, cmd) == 0))
			return 0;
	}
	return 1;
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
	msg.append(domain_ + " :Irisha server");
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
