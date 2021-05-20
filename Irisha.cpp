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
	std::cout << BOLD BWHITE "\n⭐ Server started. Waiting for the client connection. ⭐\n" CLR << std::endl;
	loop();
}

Irisha::Irisha(int port, const std::string& password)
{
	init(port);
	password_ = password;
	launch();
	print_info();
	std::cout << BOLD BWHITE "\n⭐ Server started. Waiting for the client connection. ⭐\n" CLR << std::endl;
	loop();
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
	std::cout << "Connection established! " E_FIRE "\n" << std::endl;
	print_info();
	std::cout << BOLD BWHITE "\n" E_STAR " Server started. Waiting for the client connection. " E_STAR "\n" CLR << std::endl;

	//registration
	send_msg(speaker, NO_PREFIX, createPASSmsg(network_password));
	send_msg(speaker, NO_PREFIX, createSERVERmsg());
	loop();
}

Irisha::~Irisha()
{
	close(listener_);
}

void Irisha::check_timeout_values()
{
	if (ping_timeout_ < 1 || ping_timeout_ > 10000)
	{
		ping_timeout_ = 20;
		std::cout << RED "Ping timeout is wrong - server will use default setting (20 sec)" CLR << std::endl;
	}
	if (conn_timeout_ < 1 || conn_timeout_ > 10000)
	{
		conn_timeout_ = 120;
		std::cout << RED "Connection timeout is wrong - server will use default setting (120 sec)" CLR << std::endl;
	}
	if (ping_timeout_ > conn_timeout_)
	{
		ping_timeout_ = 20;
		conn_timeout_ = 120;
		std::cout << RED "Ping timeout can't be greater than connection timeout - server will use default settings" CLR << std::endl;
	}
}

/**
 * @description	Applies config settings and checks for domain and password validity
 * @param		path: path to config
 */
void Irisha::apply_config(const std::string& path)
{
	check_config(path);
	domain_			= get_config_value(path, DOMAIN);
	welcome_		= get_config_value(path, WELCOME);
	admin_mail_		= get_config_value(path, ADMIN_MAIL);
	admin_location_	= get_config_value(path, ADMIN_LOC);
	admin_info_		= get_config_value(path, ADMIN_INFO);
	//password_		= get_config_value(path, PASS);
	ping_timeout_	= str_to_int(get_config_value(path, PING_T));
	conn_timeout_	= str_to_int(get_config_value(path, CONN_T));
	check_timeout_values();

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
	if (dots < 1)
		throw std::runtime_error("Config error: server domain must contain at least one dot.");
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
	launch_time_ = time(nullptr);
}

void Irisha::init(int port)
{
	apply_config(CONFIG_PATH);
	prepare_commands();
	listener_ = socket(PF_INET, SOCK_STREAM, 0);
	if (listener_ == -1) throw std::runtime_error("Socket creation failed!");

	signal(SIGPIPE, SIG_IGN);
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

	std::cout << E_PAGER ITALIC PURPLE " New connection from socket №" << sock << CLR << std::endl;
	return sock;
}

/**
 * @description	Launches the main server loop, which listens for
 * 				new clients, sends and receives messages
 */
void Irisha::loop()
{
	int							n;
	std::string					client_msg;
	std::list<Irisha::RegForm*>	reg_expect;	// Not registered connections
    std::deque<std::string>		arr_msg;	// Array messages, not /r/n
    timeval						timeout;	// Select timeout
    time_t						last_ping = time(nullptr);	// Time of the last connection ping

	timeout.tv_sec	= ping_timeout_;
	timeout.tv_usec	= 0;
	std::thread	sender(sending_loop, this); //! TODO: REMOVE ////////////////////////////////////////////////////////////////////////////////////////////
	while (true)
	{
		read_fds_ = all_fds_;
		n = select(max_fd_ + 1, &read_fds_, nullptr, nullptr, nullptr); //! TODO: change last nullptr to &timeout (nullptr is for debugging)
		if (n == -1) throw std::runtime_error("Select error");
//		if (difftime(time(nullptr), last_ping) >= ping_timeout_)
//			ping_connections(last_ping);

		for (int i = 3; i < max_fd_ + 1; ++i)
		{
			if (FD_ISSET(i, &read_fds_))
			{
				if (i == listener_)
				{
					int connection_fd = accept_connection();
					reg_expect.push_back(new RegForm(connection_fd));
				}
				else
				{
					client_msg = get_msg(i, reg_expect);
                    parse_arr_msg(arr_msg, client_msg);
                    while (!arr_msg.empty())
                    {
                        parse_msg(arr_msg[0], cmd_); //! TODO: fix EXC_BAD_ACCESS in cases: ":prefix", ":", "::"
						print_cmd(PM_LINE, i);
						arr_msg.pop_front();
						std::list<RegForm*>::iterator it = expecting_registration(i, reg_expect);	// Is this connection waiting for registration? TODO: add timeout handling to registration
						if (it != reg_expect.end())														// Yes, register it TODO: handle sudden user disconnection (CTRL + C, CTRL + \, CTRL + D, CTRL + Z)
						{
							if (register_connection(it) == R_SUCCESS)
							{
								RegForm* rf = *it;
								reg_expect.erase(it);
								delete rf;
								continue;
							}
						}
						handle_command(i);															// No, handle not registration command
					}
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
void Irisha::handle_disconnection(const int sock)
{
	User*		user = find_user(sock);
	std::string msg;

	if (user == nullptr)
	{
		Server*	server = find_server(sock);
		sys_msg(E_BOOM, "Server", server->name(), "disconnected!");
		send_servers(server->name(), msg);
	}
	else
	{
		msg = sys_msg(E_SCULL, "User", user->nick(), "disconnected!");
		send_servers(user->nick(), msg);
		remove_user(user->nick());
	}
	FD_CLR(sock, &all_fds_);
	close(sock);
}

/// Commands+



///Commands-

/**
 *
 * @param i - socket to identify connection
 * @param reg_expect - list of not registered connections
 * @return	node iterator, if socket in list, else end iterator
 */
std::list<Irisha::RegForm*>::iterator Irisha::expecting_registration(int i, std::list<RegForm*>& reg_expect)
{
	std::list<RegForm*>::iterator it;
	for (it = reg_expect.begin(); it != reg_expect.end(); it++)
	{
		if ((*it)->socket_ == i)
			return it;
	}
	return (reg_expect.end());
}


///TODO: handle client connection
int			Irisha::register_connection	(std::list<Irisha::RegForm*>::iterator rf)
{
	if ((*rf)->pass_received_ == false)
	{
		if (cmd_.command_ == "PASS" && (PASS((*rf)->socket_) == R_SUCCESS))
			(*rf)->pass_received_ = true;
		return R_FAILURE;
	}
	else
	{
		if (cmd_.command_ == "SERVER" && (SERVER((*rf)->socket_) == R_SUCCESS))
			return R_SUCCESS;
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
 *
 * PASS secret 0210 Irisha|
 * SERVER pig.irisha.net :Irisha server
 */
