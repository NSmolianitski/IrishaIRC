//
// Created by Parfait Kentaurus on 5/14/21.
//

#include <sstream>
#include "Irisha.hpp"
#include "User.hpp"
#include "Server.hpp"
#include "utils.hpp"
#include "parser.hpp"
#include "Channel.hpp"
/**
 * @description	Inserts all supported IRC commands to map
 */
void	Irisha::prepare_commands()
{
	commands_.insert(std::pair<std::string, func>("PASS", &Irisha::PASS));
	commands_.insert(std::pair<std::string, func>("NICK", &Irisha::NICK));
	commands_.insert(std::pair<std::string, func>("USER", &Irisha::USER));
	commands_.insert(std::pair<std::string, func>("SERVER", &Irisha::SERVER));
	commands_.insert(std::pair<std::string, func>("PING", &Irisha::PING));
	commands_.insert(std::pair<std::string, func>("PONG", &Irisha::PONG));
	commands_.insert(std::pair<std::string, func>("JOIN", &Irisha::JOIN));
	commands_.insert(std::pair<std::string, func>("MODE", &Irisha::MODE));
	commands_.insert(std::pair<std::string, func>("PART", &Irisha::PART));
	commands_.insert(std::pair<std::string, func>("QUIT", &Irisha::QUIT));
	commands_.insert(std::pair<std::string, func>("TOPIC", &Irisha::TOPIC));
	commands_.insert(std::pair<std::string, func>("PRIVMSG", &Irisha::PRIVMSG));
	commands_.insert(std::pair<std::string, func>("TIME", &Irisha::TIME));
}

/**
 * @description	Changes user nick if it's not busy
 * @param		connection: User or Server
 * @param		sock: socket
 * @param		new_nick: desired nickname
 * @return		R_SUCCESS or R_FAILURE
 */
eResult	Irisha::NICK_user(User* const connection, const int sock, const std::string& new_nick)
{
	User*		user;
	std::string	old_nick;

	old_nick = connection->nick();
	if (old_nick == new_nick)
		return R_SUCCESS;
	user = find_user(new_nick);
	if (user)
	{
		send_msg(sock, domain_, new_nick + " :Nickname is already in use"); //! TODO: change to error reply
		return R_FAILURE;
	}
	connection->set_nick(new_nick);
	send_msg(sock, old_nick, "NICK " + new_nick); // Reply for user TODO: check prefix
	// TODO: send message to next server

	std::cout << ITALIC PURPLE E_GEAR " User " BWHITE + old_nick + PURPLE " changed nick to " BWHITE
				 + new_nick << " " CLR << std::endl;
	return R_SUCCESS;
}

/**
 * @description	Handles request from other server (changes nick or adds new user)
 * @param		new_nick: new nickname
 * @return		R_SUCCESS
 */
eResult	Irisha::NICK_server(const std::string& new_nick)
{
	User*		user;
	std::string	old_nick;

	user = find_user(cmd_.prefix_);
	if (user == nullptr)		// Add new external user
	{
		add_user();
		return R_SUCCESS;
	}
	old_nick = user->nick();
	user->set_nick(new_nick);	// Change nick for external user
	// TODO: send message to next server

	std::cout << ITALIC PURPLE E_GEAR "User " BWHITE + old_nick + PURPLE " changed nick to " BWHITE
				 + new_nick << " " CLR << std::endl;
	return R_SUCCESS;
}

/**
 * @description	Handles NICK command
 * @param		sock: command sender socket
 */
eResult	Irisha::NICK(const int sock) //! TODO: handle hopcount
{
	if (cmd_.arguments_.empty())	// NICK command without params
	{
		send_msg(sock, domain_, "431 :No nickname given"); //! TODO: change to error reply
		return R_FAILURE;
	}
	std::string new_nick = cmd_.arguments_[0];
	if (!is_a_valid_nick(new_nick))
	{
		send_msg(sock, domain_, "432 :" + new_nick + " :Erroneous nickname"); //! TODO: change to error reply
		return R_FAILURE;
	}

	con_it it = connections_.begin();
	for (; it != connections_.end(); ++it)
	{
		if (it->second->socket() == sock)
			break;
	}
	if (it == connections_.end())	// Add new local user
		add_user(sock, new_nick);
	else
	{
		User*	connection = dynamic_cast<User *>(it->second);
		if (connection == nullptr)	// Handle request from other server
			return NICK_server(new_nick);
		return NICK_user(connection, sock, new_nick); // Change local user nickname
	}
	print_user_list(); //! TODO: remove
	return R_SUCCESS;
}

/**
 * @description	Handles USER command
 * @param		sock: command sender socket
 */
eResult Irisha::USER(const int sock)
{
	if (cmd_.arguments_.size() < 4)
	{
		send_msg(sock, domain_, "461 :USER :Not enough parameters"); //! TODO: change to error reply
		return R_FAILURE;
	}
	User*	user = find_user(sock);
	if (user == nullptr)	// Safeguard for invalid user
	{
		std::cout << RED BOLD "ALARM! WE DON'T HAVE USER WITH SOCKET" BWHITE " №" << sock << RED " IN OUR DATABASE!" CLR << std::endl;
		return R_FAILURE;
	}
	if (!user->username().empty())
	{
		send_msg(sock, domain_, "462 :Unauthorized command (already registered)"); //! TODO: change to error reply
		return R_FAILURE;
	}
	user->set_username(cmd_.arguments_[0]);

	if (cmd_.arguments_[1].length() > 1) //! TODO: don't forget to handle modes
		user->set_mode(0);
	else
		user->set_mode(str_to_int(cmd_.arguments_[1]));
	send_msg(sock, domain_, "001 " + user->nick() + " " + welcome_); //! TODO: change to RPL_WELCOME

	sys_msg(E_MAN, "New local user", user->nick(), "registered!");
	// NICK <nickname> <hopcount> <username> <host> <servertoken> <umode> <realname>
	send_servers(domain_, "NICK " + user->nick() + " 1 " + user->username() + " " + user->server() + " 2 + " + user->username()); //! TODO: add server token, user modes, fix realname
	return R_SUCCESS;
}

/**
 * @description	Handling PASS message
 * @param		sock: command sender socket
 * @return		R_SUCCESS if password correct, else R_FAILURE
 */
eResult Irisha::PASS(const int sock)
{
	if (cmd_.arguments_.empty())
		return R_FAILURE;
	else if (password_ == cmd_.arguments_[0] || !cmd_.prefix_.empty())
		return R_SUCCESS;
	else
		return R_FAILURE;
}

/**
 * @description	Handling SERVER message
 * @param		sock: command sender socket
 * @return		R_FAILURE if registration successfully, else R_FAILURE
 */
eResult Irisha::SERVER(const int sock)
{
	//validation+
	std::string info = "";
	for (int i = 0; i < cmd_.arguments_.size(); i++)
	{
		if (cmd_.arguments_[i].find(':') != std::string::npos)
		{
			info = cmd_.arguments_[i];
			break ;
		}
	}
	if (info.empty())
		return R_FAILURE;
	//validation-

	int hopcount;
	if (cmd_.arguments_.empty())
		return R_FAILURE;
	if (cmd_.arguments_.size() == 1) //only server name sent
		hopcount = 1;
	else if (cmd_.arguments_.size() == 2 &&
		(cmd_.arguments_[0].find_first_not_of("0123456789") == std::string::npos)) //hopcount and server name sent
	{
		try	{ hopcount = std::stoi(cmd_.arguments_[0]); }
		catch(std::exception ex) { return R_FAILURE; }
	}
	else
		hopcount = str_to_int(cmd_.arguments_[0]); ///TODO: remove it

	AConnection* server = new Server(cmd_.arguments_[0], sock, hopcount);
	connections_.insert(std::pair<std::string, AConnection*>(cmd_.arguments_[0], server));
	PING(sock);
	sys_msg(E_LAPTOP, "Server", cmd_.arguments_[0], "registered!");
	return R_SUCCESS;
}

eResult Irisha::PONG(const int sock)
{
	if (cmd_.arguments_.empty())
		return R_FAILURE; ///TODO: send ERR_NOORIGIN
	else if (cmd_.arguments_.size() == 1)
		send_msg(sock, domain_, "PONG " + domain_);
	//else send to receiver
	return R_SUCCESS;
}

eResult Irisha::PING(const int sock)
{
	if (cmd_.arguments_.empty())
		return R_FAILURE; ///TODO: send ERR_NOORIGIN
	else if (cmd_.arguments_.size() == 1) // PINGing this server
		PONG(sock);
	//else send to receiver
	return R_SUCCESS;
}

eResult Irisha::QUIT(const int sock)
{
	User*	user;
	bool	local = true;
	if (cmd_.prefix_.empty())
		user = find_user(sock);
	else
	{
		user = find_user(cmd_.prefix_);
		local = false;
	}

	std::string	msg;
	msg = sys_msg(E_SCULL, "User", user->nick(), "disconnected!");
	if (!cmd_.arguments_.empty())
		msg = cmd_.arguments_[0];

	if (local)
	{
		send_servers(user->nick(), msg);
		FD_CLR(sock, &all_fds_);
		close(sock);
	}
	else
		send_servers(user->nick(), msg, sock);
	remove_user(user->nick());
	return R_SUCCESS;
}

eResult Irisha::MODE(const int sock) // Доделать !!!
{
    if (cmd_.arguments_.size() == 1) {
        send_msg(sock, domain_, "324 " + find_user(sock)->nick() + " " + cmd_.arguments_[0] + " +");
    }
}

int     Irisha::check_mode_channel(const Channel* channel, const int sock, std::list<std::string>& arr_key, std::string& arr_channel)
{
    CITERATOR itr = channel->getBanUsers().begin();
    CITERATOR ite = channel->getBanUsers().end();
    if (!channel->getKey().empty()){
        if (arr_key.empty() || arr_key.front() != channel->getKey()) {
            send_msg(sock, domain_, "475 " + arr_channel + " :Cannot join channel (+k)");
            arr_key.pop_front();
            return 1;
        }
        arr_key.pop_front();
    }
    while (itr != ite)
    {
        if ((*itr) == find_user(sock)){
            send_msg(sock, domain_, "474 " + arr_channel + " :Cannot join channel (+b)");
            return 1;
        }
        itr++;
    }
    if (channel->getMode().find('i')->second == 1){
        itr = channel->getInviteUsers().begin();
        ite = channel->getInviteUsers().end();
        while (itr != ite){
            if ((*itr) == find_user(sock))
                break;
            itr++;
        }
        if (itr == ite){
            send_msg(sock, domain_, "473 " + arr_channel + " :Cannot join channel (+i)");
            return 1;
        }
    }
    if (channel->getMode().find('l')->second == 1){
        if (channel->getUsers().size() >= channel->getMaxUsers()){
            send_msg(sock, domain_, "471 " + arr_channel + " :Cannot join channel (+l)");
            return 1;
        }
    }
    return 0;
}

eResult Irisha::JOIN(const int sock)
{
    std::vector<std::string> arr_channel; // array channels
    std::list<std::string> arr_key; // arrray key for channels
    std::string str_channels = cmd_.arguments_[0]; // string channels "#123,#321"

    if (cmd_.arguments_.size() == 2){
        std::string str_keys = cmd_.arguments_[1]; // string keys "hello,world"
        parse_arr_list(arr_key, str_keys, ',');
    }
    parse_arr(arr_channel, str_channels, ',');
    for (size_t i = 0; i < arr_channel.size(); ++i) {
        if ((arr_channel[i][0] == '#' || arr_channel[i][0] == '&' || arr_channel[i][0] == '+' || arr_channel[i][0] == '!')){
            std::map<std::string, Channel*>::iterator itr = channels_.find(arr_channel[i]);
            if (itr == channels_.end()){
                if (arr_channel[i].size() > 50){
                    send_msg(sock, domain_, "403 " + arr_channel[i] + " :No such channel");
                    continue;
                }
                send_msg(sock, "", ":" + find_user(sock)->nick() + " JOIN " + arr_channel[i]);
                Channel* channel = new Channel(arr_channel[i]);
                channel->setType(arr_channel[i][0]);
                channel->addOperators(find_user(sock));
                channel->addUser(find_user(sock));
                if (!arr_key.empty())
                    channel->setKey(arr_key.front());
                channels_.insert(std::pair<std::string, Channel*>(arr_channel[i], channel));
                send_msg(sock, domain_, "353 " + find_user(sock)->nick() + " = " + arr_channel[i] +" :" + channel->getListUsers());
                send_msg(sock, domain_, "366 " + find_user(sock)->nick() + " " + arr_channel[i] +" :End of NAMES list");
            }
            else{
                if (check_mode_channel((*itr).second, sock, arr_key, arr_channel[i]) == 1)
                    continue;
                send_msg(sock, "", ":" + find_user(sock)->nick() + " JOIN " + arr_channel[i]);
                itr->second->addUser(find_user(sock));
                if (itr->second->getTopic().empty())
                    send_msg(sock, domain_, "331 " + find_user(sock)->nick() + " " + arr_channel[i] + " :No topic is set");
                else
                    send_msg(sock, domain_, "332 " + find_user(sock)->nick() + " " + arr_channel[i] + " :" + itr->second->getTopic());
                send_msg(sock, domain_, "353 " + find_user(sock)->nick() + " = " + arr_channel[i] +" :" + itr->second->getListUsers());
                send_msg(sock, domain_, "366 " + find_user(sock)->nick() + " " + arr_channel[i] +" :End of NAMES list");
                send_channel((*itr).second, "JOIN " + arr_channel[i], find_user(sock)->nick());
            }
        }
    }
    return R_SUCCESS;
}

eResult Irisha::PART(const int sock)
{
    std::vector<std::string> arr_channel;
    std::string str_channels = cmd_.arguments_[0];

    parse_arr(arr_channel, str_channels, ',');
    for (size_t i = 0; i < arr_channel.size(); ++i) {
        if ((arr_channel[i][0] == '#' || arr_channel[i][0] == '&' || arr_channel[i][0] == '+' || arr_channel[i][0] == '!')){
            std::map<std::string, Channel*>::iterator itr = channels_.find(arr_channel[i]);
            if (itr == channels_.end()){
                send_msg(sock, domain_, "403 " + find_user(sock)->nick() + " " + arr_channel[i] + " :No such channel");
                continue;
            }
            CITERATOR itr_u = (*itr).second->getUsers().begin();
            CITERATOR ite_u = (*itr).second->getUsers().end();
            while (itr_u != ite_u){
                if ((*itr_u) == find_user(sock))
                    break;
                itr_u++;
            }
            if (itr_u == ite_u){
                send_msg(sock, domain_, "442 " + find_user(sock)->nick() + " " + arr_channel[i] + " :You're not on that channel");
                continue;
            }
            send_channel((*itr).second, "PART " + arr_channel[i], find_user(sock)->nick());
            (*itr).second->delUser(find_user(sock));
            if ((*itr).second->getUsers().size() == 0){
                delete((*itr).second);
                channels_.erase(itr);
            }
        }
    }
    return R_SUCCESS;
}

eResult Irisha::TOPIC(const int sock)
{
    if (cmd_.arguments_.size() > 2)
        return R_SUCCESS;
    if ((cmd_.arguments_[0][0] == '#' || cmd_.arguments_[0][0] == '&' || cmd_.arguments_[0][0] == '+' || cmd_.arguments_[0][0] == '!')){
        std::map<std::string, Channel*>::iterator itr = channels_.find(cmd_.arguments_[0]);
        if (itr == channels_.end()){
            send_msg(sock, domain_, "403 " + find_user(sock)->nick() + " " + cmd_.arguments_[0] + " :No such channel");
            return R_SUCCESS;
        }
        if (cmd_.arguments_.size() == 1){
            if (itr->second->getTopic().empty())
                send_msg(sock, domain_, "331 " + find_user(sock)->nick() + " " + cmd_.arguments_[0] + " :No topic is set");
            else
                send_msg(sock, domain_, "332 " + find_user(sock)->nick() + " " + cmd_.arguments_[0] + " :" + itr->second->getTopic());
            return R_SUCCESS;
        }
        CITERATOR itr_u = (*itr).second->getUsers().begin();
        CITERATOR ite_u = (*itr).second->getUsers().end();
        while (itr_u != ite_u){
            if (*itr_u == find_user(sock))
                break;
            itr_u++;
        }
        if (itr_u == ite_u){
            send_msg(sock, domain_, "442 " + find_user(sock)->nick() + " " + cmd_.arguments_[0] + " :You're not on that channel");
            return R_SUCCESS;
        }
        itr_u = (*itr).second->getOperators().begin();
        ite_u = (*itr).second->getOperators().end();
        while (itr_u != ite_u){
            if (*itr_u == find_user(sock))
                break;
            itr_u++;
        }
        if (itr_u == ite_u){
            send_msg(sock, domain_, "482 " + find_user(sock)->nick() + " " + cmd_.arguments_[0] + " :You're not channel operator");
            return R_SUCCESS;
        }
        if ((*itr).second->getMode().find('t')->second == 0){
            send_msg(sock, domain_, "477 " + find_user(sock)->nick() + " " + cmd_.arguments_[0] + " :Channel doesn't support modes");
            return R_SUCCESS;
        }
        cmd_.arguments_[1].erase(cmd_.arguments_[1].begin());
        if (cmd_.arguments_[1] == (*itr).second->getTopic())
            return R_SUCCESS;
        (*itr).second->setTopic(cmd_.arguments_[1]);
        send_channel((*itr).second, "TOPIC " + cmd_.arguments_[0] + " :" + cmd_.arguments_[1], find_user(sock)->nick());
    }
    return R_SUCCESS;
}

eResult Irisha::PRIVMSG(const int sock)
{
    if (cmd_.arguments_.size() == 0){
        send_msg(sock, domain_, "411 " + find_user(sock)->nick() + " " + " :No recipient given (PRIVMSG)");
        return R_SUCCESS;
    }
    if (cmd_.arguments_.size() == 1){
        send_msg(sock, domain_, "412 " + find_user(sock)->nick() + " " + " :No text to send");
        return R_SUCCESS;
    }

    std::list<std::string> arr_receiver;
    std::string str_receiver = cmd_.arguments_[0];
    User* user;

    parse_arr_list(arr_receiver, str_receiver, ',');
    arr_receiver.sort();
    arr_receiver.unique();
    while (!arr_receiver.empty()){
        if ((arr_receiver.front()[0] == '#' || arr_receiver.front()[0] == '&' || arr_receiver.front()[0] == '+' || arr_receiver.front()[0] == '!')){
            std::map<std::string, Channel*>::iterator itr = channels_.find(arr_receiver.front());
            if (itr == channels_.end()){
                send_msg(sock, domain_, "401 " + find_user(sock)->nick() + " " + arr_receiver.front() + " :No such nick/channel");
                arr_receiver.pop_front();
                continue;
            }
            CITERATOR itr_u = (*itr).second->getUsers().begin();
            CITERATOR ite_u = (*itr).second->getUsers().end();
            while (itr_u != ite_u){
                if (*itr_u == find_user(sock))
                    break;
                itr_u++;
            }
            if (itr_u == ite_u && (*itr).second->getMode().find('n')->second == 1){
                send_msg(sock, domain_, "404 " + find_user(sock)->nick() + " " + arr_receiver.front() + " :Cannot send to channel");
                arr_receiver.pop_front();
                continue;
            }
            itr_u = (*itr).second->getModerators().begin();
            ite_u = (*itr).second->getModerators().end();
            while (itr_u != ite_u){
                if (*itr_u == find_user(sock))
                    break;
                itr_u++;
            }
            if (itr_u == ite_u && (*itr).second->getMode().find('m')->second == 1){
                send_msg(sock, domain_, "404 " + find_user(sock)->nick() + " " + arr_receiver.front() + " :Cannot send to channel");
                arr_receiver.pop_front();
                continue;
            }
            send_channel((*itr).second, "PRIVMSG " + arr_receiver.front() + " " + cmd_.arguments_[1], find_user(sock)->nick(), sock);
        } else {
            user = find_user(arr_receiver.front());
            if (user != nullptr){
                send_msg(sock, domain_, "401 " + find_user(sock)->nick() + " " + arr_receiver.front() + " :No such nick/channel");
                arr_receiver.pop_front();
                continue;
            }
            send_msg(sock, find_user(sock)->nick(), "PRIVMSG " + arr_receiver.front() + " " + cmd_.arguments_[1]);
        }
        arr_receiver.pop_front();
    }
    return R_SUCCESS;
}

//ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
//ERR_WILDTOPLEVEL

eResult Irisha::TIME(const int sock)
{
	time_t	current_time = time(nullptr);
	std::string local_time = ctime(&current_time);
	local_time = local_time.substr(0, local_time.length() - 1);
	if (!cmd_.arguments_.empty())
	{
		if (find_server(cmd_.arguments_[0]) == nullptr)
		{
			err_nosuchserver(sock, cmd_.arguments_[0]);
			return R_FAILURE;
		}
		rpl_time(sock, cmd_.arguments_[0], local_time);
		return R_SUCCESS;
	}
	rpl_time(sock, domain_, local_time);
	return R_SUCCESS;
}
