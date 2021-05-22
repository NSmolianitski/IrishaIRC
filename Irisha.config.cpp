
#include "Irisha.hpp"

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
	oper_pass_		= str_to_int(get_config_value(path, OPER_PASS));
	set_time_stamp(path);

	check_timeout_values();
	check_domain();
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

void Irisha::check_domain()
{
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

void Irisha::set_time_stamp(const std::string& path)
{
	std::string	value = get_config_value(path, TIME_STAMP);
	if (value == "yes")
		time_stamp_		= U_ENABLED;
	else if (value == "no")
		time_stamp_		= U_DISABLED;
	else
		throw std::runtime_error("Config error: wrong time-stamp value");
}
