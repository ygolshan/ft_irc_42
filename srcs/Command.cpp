/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akhodara <akhodara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 12:48:51 by ygolshan          #+#    #+#             */
/*   Updated: 2024/03/26 20:25:19 by akhodara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Command.hpp"
#include "../includes/Client.hpp"
#include "../includes/Define.hpp"
#include "../includes/Channel.hpp"

class	Server;

Command::Command() {}

Command::Command( const Command &c )
{
	(void)c;
}

Command::~Command() {}

Command	&Command::operator=( const Command &c )
{
	(void)c;
	return (*this);
}

Command	&Command::getInstance()
{
	static Command	instance;

	return (instance);
}

void Command::nick( Server *server, Client *client, std::istringstream &buffer_stream )
{
	std::string 		name;
	std::string 		pre_nick;
	std::string			pre_prefix;

	if (!(buffer_stream >> name))
		client->setSendData(ERR_NONICKNAMEGIVEN(client->getNick()));
	else
	{
		if (server->is_ClientIn_Channel(name))
			client->setSendData(ERR_NICKNAMEINUSE(client->getNick(), name) + CRLF);
		else
		{
			pre_nick = client->getNick();
			pre_prefix = client->get_Client_Prefix();
			client->setNick(name);
			server->update_Channel_Nickname(*client, pre_nick, pre_prefix);
			
			client->setSendData("NICK :" + name + "\n\r\n");
			std::cout << "< Client " << client->getSocket() << " > nickname setted: " << name << std::endl;
		}
	}
}

void	Command::_removeCRLF( std::string &str )
{
	size_t	pos;

	pos = str.find('\r');
	if (pos != std::string::npos)
		str.erase(pos, 1);
	pos = str.find('\n');
	if (pos != std::string::npos)
		str.erase(pos, 1);
}

void	Command::quit( Server *server, Client *client, std::istringstream &iss )
{
	std::string		message;

	std::getline(iss, message);

	if (message.empty())
		message = ":leaving";

	_removeCRLF(message);

	client->setSendData(ERR_QUIT(client->get_Client_Prefix(), message) + CRLF);

	std::string	res;

	res = RPL_QUIT(client->get_Client_Prefix(), message) + CRLF;

	ChannelMap channels = client->retrieve_Channels();

	for (ChannelMap::iterator it = channels.begin(); it != channels.end(); it++)
	{
		std::string	channelName = it->second->getName();
		server->broadcast_Message(channelName, res);
	}

	client->setClosed(true);
}

void	Command::_split( Client *client, std::istringstream &iss, std::string *split )
{
	int			i = 0;
	std::string	line;

	while (i < 4)
	{
		if (!(iss >> line))
		{
			client->setSendData(ERR_NEEDMOREPARAMS(client->getNick(), std::string("USER")) + CRLF);
			return ;
		}

		split[i++] = line;
	}

	if (split[3][0] == ':')
	{
		split[3] = split[3].substr(1);

		if (split[3].length() == 0)
			iss >> split[3];
	}
	std::cout << " real name : " << split[3] << std::endl;
}

void	Command::user( Client *client, std::istringstream &iss )
{
	std::string	line;
	std::string	split[4];

	this->_split(client, iss, split);
	std::cout << std::endl;

	if (split[0].empty() || split[1].empty() || split[2].empty() || split[3].empty())
	{
		client->setSendData(ERR_NEEDMOREPARAMS(client->getNick(), std::string("USER")) + CRLF);
		return ;
	}

	client->setUserName(split[0]);
	client->setHostName(split[1]);
	client->setServerName(split[2]);
	client->setRealName(split[3]);

	std::cout << "< Client " << client->getSocket() << " > user information set" << std::endl;

	client->setSendData(RPL_WELCOME(client->getNick()) + CRLF);
	client->append_To_Send_Data(RPL_YOURHOST(client->getHostName()) + CRLF + CRLF);
}

void	Command::topic( Server *server, Client *client, std::istringstream &iss )
{
	std::string		channelName;
	std::string		topic;

	iss >> channelName;
	std::getline(iss, topic);

	this->_removeCRLF(topic);

	if (channelName[0] == ':')
		channelName = channelName.substr(1);
	std::cout << "channelName: " << channelName << std::endl;
	if (topic[0] == ' ' || topic[0] == ':')
		topic = topic.substr(1);

	if (channelName.empty())
	{
		client->setSendData(ERR_NEEDMOREPARAMS(client->getNick(), std::string("TOPIC")) + CRLF);
		return ;
	}

	Channel *channel = server->retrieve_Channel(channelName);
	if (!channel)
	{
		client->setSendData(ERR_NOSUCHCHANNEL(client->getNick(), channelName) + CRLF);
		return ;
	}

	if (!channel->is_ClientIn_Channel(client->getNick()))
	{
		client->setSendData(ERR_NOTONCHANNEL(client->getNick(), channelName) + CRLF);
		return ;
	}

	if (topic.empty())
	{
		if (channel->get_Channel_Topic().empty())
		{
			client->setSendData(RPL_NOTOPIC(client->getNick(), channelName) + CRLF);
		}
		else
		{
			std::cout << "get_Channel_Topic(): " << channel->get_Channel_Topic() << std::endl;
			std::cout << "get_Topic_Setter(): " << channel->get_Topic_Setter() << std::endl;
			std::cout << "get_Channel_TopicSetTime(): " << channel->get_Channel_TopicSetTime() << std::endl;
			client->setSendData(RPL_TOPIC(client->getNick(), channelName, channel->get_Channel_Topic()) + CRLF);
			client->append_To_Send_Data(RPL_TOPICWHOTIME(client->getNick(), channelName, channel->get_Topic_Setter(), channel->get_Channel_TopicSetTime()) + CRLF + CRLF);
		}

		return ;
	}
	if (!channel->isOperator(*client) && channel->checkmode('t'))
	{
		client->setSendData(ERR_CHANOPRIVSNEEDED(channelName) + CRLF + CRLF);
		return ;
	}

	channel->setTopic(*client, topic);
	server->broadcast_Message(channelName, RPL_TOPIC2(client->getNick(), channelName, topic) + CRLF);
}

void	Server::set_Non_Blocking( int fd )
{
	int	flag;

	flag = fcntl(fd, F_SETFL, O_NONBLOCK);
	if (flag == ERROR)
		throw std::runtime_error("Erorr");
}

void	Server::_free()
{
	if (this->_serverSoc != FD_ERROR)
		close(this->_serverSoc);
	if (this->_kq != FD_ERROR)
		close(this->_kq);
}

void	Server::_exit( const char *errmsg )
{
	this->_free();
	std::cerr << errmsg << std::endl;
	exit(FAILURE);
}
void Command::remove_Client_From_Channel( Server *server, Client *client, Channel *channel )
{
	std::string ch_name = channel->getName();
	server->broadcast_Message(ch_name, RPL_PART(client->getNick(), client->getNick(), ch_name));
	client->remove_joined_Channel(ch_name);
	channel->remove_Client_From_Channel(client->getNick());
	if (channel->retrieve_Clients().empty())
	{
		server->retrieve_Channels().erase(ch_name);
		delete channel;
		channel = 0;
	}
}

void Command::part( Server *server, Client *client, std::istringstream &buffer_stream )
{
	std::string response;
	std::string channel_line;

	buffer_stream >> channel_line;

	std::vector<std::string> v_channels;

	std::istringstream new_stream(channel_line);
	std::string channel;
	while (std::getline(new_stream, channel, ','))
	{
		this->_removeCRLF(channel);
		v_channels.push_back(channel);
	}

	for (std::vector<std::string>::iterator it = v_channels.begin(); it != v_channels.end(); it++)
	{
		if (server->retrieve_Channels().find(*it) == server->retrieve_Channels().end())
		{
			client->setSendData(ERR_NOSUCHCHANNEL2(client->getNick(), *it) + CRLF + CRLF);
			return ;
		}
		Channel *ch_po = server->retrieve_Channels()[*it];

		Client	*cl = ch_po->retrieve_Client(client->getNick());

		if (!cl)
		{
			client->setSendData(ERR_NOTONCHANNEL2(client->getNick(), *it) + CRLF + CRLF);
			return ;
		}

		this->remove_Client_From_Channel(server, client, ch_po);
		client->append_To_Send_Data(ERR_NOTONCHANNEL2(client->getNick(), *it) + CRLF + CRLF);
	}
}

void	Command::pass( Server *server, Client *client, std::istringstream &iss )
{
	std::string		password;

	if (client->isRegistered())
	{
		client->setSendData(ERR_ALREADYREGISTRED(client->getNick()));
		client->append_To_Send_Data(CRLF);
		return ;
	}

	iss >> password;

	if (password.empty())
	{
		client->setSendData(ERR_NEEDMOREPARAMS(client->getNick(), std::string("PASS")));
		client->append_To_Send_Data(CRLF);
		return ;
	}

	if (password != server->get_Channel_Password())
	{
		client->setSendData(ERR_PASSWDMISMATCH(client->getNick()));
		client->append_To_Send_Data(CRLF);
		return ;
	}

	client->setSendData("Password accepted\n\r\n");
	client->setRegistered(true);

	std::cout << "< Client " << client->getSocket() << " > PASS success" << std::endl;
}

void	Command::ping( Client *client, std::istringstream &iss )
{
	std::string		serverName;

	iss >> serverName;

	if (serverName.empty())
	{
		client->setSendData(ERR_NOORIGIN(client->getNick()) + CRLF);
		return ;
	}

	client->setSendData(RPL_PONG(client->getNick(), serverName) + CRLF);
}
void	Command::privmsg( Server *server, Client *client, std::istringstream &iss )
{
	std::string	target;
	std::string	message;

	iss >> target;

	std::getline(iss, message);

	if (target.empty())
	{
		client->setSendData(ERR_NEEDMOREPARAMS(client->getNick(), std::string("PRIVMSG")));
		client->append_To_Send_Data(CRLF);
		return ;
	}

	if (target[0] == '#' || target[0] == '&')
		this->transmit_To_Channel(server, client, target, message);
	else
		this->transmit_To_Client(server, client, target, message);
}

void	Command::transmit_To_Channel(
	Server *server, Client *client,
	std::string &target, std::string &message )
{
	std::cout << "SERVER PRIVMSG to Channel " << message << std::endl;

	Channel	*channel;
	std::string	prefix;

	channel = server->retrieve_Channel(target);
	if (!channel)
	{
		client->setSendData(ERR_NOSUCHCHANNEL(client->getNick(), target) + CRLF);
		return ;
	}

	if (!channel->is_ClientIn_Channel(client->getNick()))
	{
		std::cout << "channel name: " << channel->getName() << "\n";
		std::cout << "client  name: " << client->getNick() << "\n";
		client->setSendData(ERR_CANNOTSENDTOCHAN(client->getNick(), target) + CRLF);
		return ;
	}

	ClientSet			clients = channel->retrieve_Clients();
	ClientSet::iterator	it = clients.begin();

	for (; it != clients.end(); it++)
	{
		if ((*it)->getNick() == client->getNick())
			continue ;

		(*it)->setSendData(RPL_PRIVMSG(client->get_Client_Prefix(), target, message) + "\n");
	}
}

void	Command::transmit_To_Client(
	Server *server, Client *client,
	std::string &target, std::string &message )
{
	std::cout << "\nSERVER PRIVMSG to Client" << message << std::endl;

	Client	*tarretrieve_Client;

	if (!server->is_ClientIn_Channel(target))
	{
		client->setSendData(ERR_NOSUCHNICK(client->getNick(), target) + CRLF);
		return ;
	}

	tarretrieve_Client = server->retrieve_Client(target);
	tarretrieve_Client->setSendData(
		RPL_PRIVMSG(client->get_Client_Prefix(), target, message) + "\n");
}