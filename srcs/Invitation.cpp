/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invitation.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akhodara <akhodara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 12:42:23 by ygolshan          #+#    #+#             */
/*   Updated: 2024/03/26 20:25:22 by akhodara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Command.hpp"
#include "../includes/Client.hpp"
#include "../includes/Define.hpp"
#include "../includes/Channel.hpp"

void	Command::invite( Server *server, Client *client, std::istringstream &iss )
{
	std::string	target;
	std::string	channelName;

	iss >> target;
	iss >> channelName;

	Channel	*ch = client->find_joined_Channel(channelName);
	if (!ch)
	{
		client->setSendData(ERR_NOTONCHANNEL(client->getNick(), channelName) + CRLF + CRLF);
		return ;
	}

	Client	*cl = server->retrieve_Client(target);
	if (!cl)
	{
		client->setSendData(ERR_NOSUCHNICK(client->getNick(), target) + CRLF + CRLF);
		return ;
	}
	
	if (ch->is_ClientIn_Channel(cl->getNick()))
	{
		client->setSendData(ERR_USERONCHANNEL(client->getNick(), channelName, target) + CRLF + CRLF);
		return ;
	}

	if (!ch->isOperator(*client))
	{
		client->setSendData(ERR_CHANOPRIVSNEEDED(channelName) + CRLF + CRLF);
		return ;
	}

	cl->add_invited_Channel(ch);
	ch->get_Auth(cl->getNick());
	cl->setSendData(RPL_invite(client->getNick(), target, channelName) + CRLF);
}

class Server;

std::string Command::execute_Channel_join_Requests(Server *server, Client &client, const std::string &channelName, const std::string &accessKey, std::string &result)
{
    std::vector<std::string> channels;
    std::stringstream channelStream(channelName);
    std::string currentChannel;

    while (std::getline(channelStream, currentChannel, ','))
    {
        currentChannel.erase(std::remove(currentChannel.begin(), currentChannel.end(), '\r'));
        currentChannel.erase(std::remove(currentChannel.begin(), currentChannel.end(), '\n'));
        channels.push_back(currentChannel);
    }

    std::vector<std::string> accessKeys;
    std::stringstream keyStream(accessKey);
    std::string currentKey;

    while (std::getline(keyStream, currentKey, ','))
    {
        currentKey.erase(std::remove(currentKey.begin(), currentKey.end(), '\r'));
        currentKey.erase(std::remove(currentKey.begin(), currentKey.end(), '\n'));
        accessKeys.push_back(currentKey);
    }

    unsigned int index = 0;

    for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); it++)
    {
		Channel *channel = server->retrieve_Channel(*it);
		if (channel && channel->retrieve_Client(client.getNick()) != NULL)
		{
			result += ERR_USERONCHANNEL(client.getNick(), client.getNick(), *it) + CRLF;
			return result;
		}

        if (index < accessKeys.size()){
			std::string key = accessKeys[index];
            result += manage_Client_join_Channel(server, client, *it, accessKeys[index]);
		}
        else{
			std::string key = "";
            result += manage_Client_join_Channel(server, client, *it, key);

		}
        index++;
    }
	return result;
}

void Command::join(Server *server, Client *client, std::istringstream &iss)
{

    std::string result;

    std::string channelName;
    std::string accessKey;

    iss >> channelName;
    iss >> accessKey;

    if (channelName.empty())
        result += ERR_NEEDMOREPARAMS(client->getNick(), "join") + CRLF;

    else if (client->retrieve_Channels().size() > 10)
        result += ERR_TOOMANYCHANNELS(client->getNick()) + CRLF;

	else
	{
		if (channelName[0] != '#' && channelName[0] != '&')
			channelName = "#" + channelName;
		result = execute_Channel_join_Requests(server, *client, channelName, accessKey, result);
		std::cout << result << std::endl;
	}

	client->setSendData(result + CRLF);
}

std::string add_CRLF2(std::string buffer)
{
	return buffer += "\r\n";
}

std::string Command::manage_Client_join_Channel(Server *server, Client &client, std::string &ch_name, std::string &key)
{
	std::string 	response;
	Channel 		*p_channel;

    if (server->isChannel(ch_name))
        p_channel = server->retrieve_Channel(ch_name);
    else
    {
		p_channel = server->initialize_Channel(ch_name, key, client);
	}

	if (p_channel->retrieve_Clients().size() + 1 > p_channel->get_User_Limit())
	{
		response += ERR_CHANNELISFULL(client.getNick(), p_channel->getName()) + CRLF;
		return response;
	}

	if ((!p_channel->get_Channel_Password().empty() && key.empty())
		|| (!p_channel->get_Channel_Password().empty() && key != p_channel->get_Channel_Password())
		|| (p_channel->get_Channel_Password().empty() && !key.empty()))
	{
		response += ERR_BADCHANNELKEY(client.getNick(), ch_name) + CRLF;
		return response;
	}

	if (p_channel->check_invite_Mode() && !p_channel->check_Invitation(client.getNick()))
	{
		response += ERR_inviteONLYCHAN(client.getNick(), ch_name) + CRLF;
		return response;
	}

	std::string s_users = "";
	std::set<Client *> users = p_channel->retrieve_Clients();

	if (users.find(&client) == users.end())
		p_channel->joinClient(client, "NORMAL");

	client.join_Channel(p_channel);

	for (std::set<Client *>::iterator it = users.begin(); it != users.end(); ++it)
		s_users.append((*it)->getNick() + " ");
	server->broadcast_Message(ch_name, RPL_join(client.get_Client_Prefix(), ch_name) + CRLF);

	std::string who_in_channel = "channel : ";
    std::set<Client *> channel_in_user = p_channel->retrieve_Clients();
	for (std::set<Client *>::iterator it = channel_in_user.begin(); it != channel_in_user.end(); ++it)
	{
		Client *currentClient = *it;
		who_in_channel +=  " " + currentClient->getNick();
	}
	std::cout << who_in_channel << std::endl;

	if (!p_channel->get_Channel_Topic().empty())
	{
		std::string msg1 = RPL_TOPIC2(client.getNick(), ch_name, p_channel->get_Channel_Topic());
		std::string msg2 = RPL_TOPICWHOTIME(client.getUserName(), ch_name, p_channel->get_Topic_Setter(), p_channel->get_Channel_TopicSetTime());
		response += add_CRLF2(msg1);
		response += add_CRLF2(msg2);
	}

	std::string msg3 = RPL_NAMREPLY(client.getNick(), ch_name, s_users);
	response += add_CRLF2(msg3);

	std::string msg4 = RPL_ENDOFNAMES(client.getNick(), ch_name);
	response += add_CRLF2(msg4);

	return response;
}