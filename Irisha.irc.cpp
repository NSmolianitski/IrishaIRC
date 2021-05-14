//
// Created by Parfait Kentaurus on 5/14/21.
//

#include "Irisha.hpp"
#include "User.hpp"
#include "utils.hpp"
#include "parser.hpp"
#include "Channel.hpp"
/**
 * @description	handles NICK command
 * @param cmd: Command structure
 * @param socket: command sender
 */
void Irisha::NICK(const Command& cmd, const int sock) //! TODO: handle hopcount
{
	if (cmd.arguments.empty()) // NICK command without params
	{
		send_msg(sock, domain_, ":No nickname given"); //! TODO: change to error reply
		return;
	}
	std::string new_nick = cmd.arguments[0];
	std::map<int, AConnection*>::iterator it = connections_.find(sock);

	if (it == connections_.end())	// Add new user
	{
		if (!is_a_valid_nick(new_nick))
			send_msg(sock, domain_, new_nick + " :Erroneus nickname"); //! TODO: change to error reply
		add_user(sock, new_nick);
	}
	else							// Change nickname
	{
		User* user = dynamic_cast<User *>(it->second);
		if (user == nullptr) // if user is not local
		{
			find_user(cmd.prefix)->set_nick(new_nick);
			// TODO: send message to next server
		}
		else
		{
			if (find_user(new_nick))
			{
				send_msg(sock, domain_, new_nick + " :Nickname is already in use"); //! TODO: change to error reply
				return;
			}
			user->set_nick(new_nick);
		}
	}
}

void Irisha::JOIN(const Command& cmd, const int sock)
{
    std::vector<std::string> arr_channel;
    std::string str_channels = cmd.arguments[0];

    parse_arr(arr_channel, str_channels, ',');
    for (size_t i = 0; i < arr_channel.size(); ++i) {
        if (arr_channel[i][0] == '#' || arr_channel[i][0] == '&'){
            //send_msg(sock, "", ":localhost 332 Dmitriy " + arr_channel[i] +" :Hello");
            Channel* channel = new Channel(arr_channel[i]);
            std::cout << arr_channel[i] << std::endl;
            send_msg(sock, "", ":Dmitriy JOIN " + arr_channel[i]);
            send_msg(sock, domain_, "353 Dmitriy = " + arr_channel[i] +" :@Dmitriy");
            send_msg(sock, domain_, "366 Dmitriy " + arr_channel[i] +" :End of NAMES list");
        }
    }

}