/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akhodara <akhodara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 12:45:40 by ygolshan          #+#    #+#             */
/*   Updated: 2024/03/26 20:25:25 by akhodara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Command.hpp"
#include "../includes/Client.hpp"
#include "../includes/Define.hpp"
#include "../includes/Channel.hpp"

void	Command::kick( Server *server, Client *client, std::istringstream &iss )
{
	std::string		channelName;
	std::string		targets;
	std::string		message;

	iss >> channelName;
	iss >> targets;
	iss >> message;

	std::cout << "channelName: " << channelName << std::endl;
	std::cout << "targets: " << targets << std::endl;
	std::cout << "message: " << message << std::endl;
	

	if (channelName.empty() || targets.empty())
	{
		client->setSendData(ERR_NEEDMOREPARAMS(client->getNick(), std::string("KICK")) + CRLF + CRLF);
		return ;
	}

	Channel	*channel = server->retrieve_Channel(channelName);
	if (!channel)
	{
		client->setSendData(ERR_NOSUCHCHANNEL(client->getNick(), channelName) + CRLF + CRLF);
		return ;
	}

	std::cout << "channelName: " << channelName << std::endl;

	if (!channel->is_ClientIn_Channel(client->getNick()))
	{
		client->setSendData(ERR_NOTONCHANNEL(client->getNick(), channelName) + CRLF + CRLF);
		return ;
	}

	if (!channel->isOperator(*client))
	{
		client->setSendData(ERR_CHANOPRIVSNEEDED(channelName) + CRLF + CRLF);
		return ;
	}

	while (!targets.empty())
	{
		std::string		target;
		size_t			pos = targets.find(',');

		if (pos == std::string::npos)
		{
			target = targets;
			targets.clear();
		}
		else
		{
			target = targets.substr(0, pos);
			targets = targets.substr(pos + 1);
		}

		std::cout << "KICK target: " << target << std::endl;

		if (!channel->is_ClientIn_Channel(target))
		{
			client->append_To_Send_Data(ERR_USERNOTINCHANNEL(client->getNick(), target, channelName) + CRLF + CRLF);
			continue ;
		}

		Client	*tarretrieve_Client = server->retrieve_Client(target);
		channel->remove_Client_From_Channel(tarretrieve_Client->getNick());
		channel->remove_Client_Authorization(tarretrieve_Client->getNick());
		tarretrieve_Client->remove_joined_Channel(channelName);
		target += "	";
		server->broadcast_Message(channelName, RPL_KICK(client->getNick(), channelName, target, message) + CRLF + CRLF);
		tarretrieve_Client->setSendData(RPL_KICK(client->getNick(), channelName, target, message) + CRLF + CRLF);
	}
}