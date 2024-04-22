/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akhodara <akhodara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 12:40:55 by ygolshan          #+#    #+#             */
/*   Updated: 2024/03/26 20:25:13 by akhodara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Command.hpp"
#include "../includes/Client.hpp"
#include "../includes/Define.hpp"
#include "../includes/Channel.hpp"

Channel::Channel() : _isinviteOnly(false), _isTopicRestricted(false) {
    this->_accessLimit = 1000000;
}

Channel::Channel(const Channel &c)
    : _isinviteOnly(c._isinviteOnly), _isTopicRestricted(c._isTopicRestricted),
      _channelName(c._channelName), _topic(c._topic), _password(c._password),
      _clients(c._clients) {}

Channel &Channel::operator=(const Channel &c)
{
    if (this != &c)
    {
        _isinviteOnly = c._isinviteOnly;
        _isTopicRestricted = c._isTopicRestricted;
        _channelName = c._channelName;
        _topic = c._topic;
        _password = c._password;
        _clients = c._clients;
        _operators = c._operators;
    }
    return *this;
}

bool    Channel::operator<( const Channel &c ) const
{
    return (this->_channelName < c._channelName);
}

bool	Channel::operator<=( const Channel &c ) const
{
	return (this->_channelName <= c._channelName);
}

bool	Channel::operator>( const Channel &c ) const
{
	return (this->_channelName > c._channelName);
}

bool	Channel::operator>=( const Channel &c ) const
{
	return (this->_channelName >= c._channelName);
}


bool Channel::isOwner(Client& client)
{
	if (this->_clientAuth[client.getNick()] == "OWNER")
		return true;
	return false;
}

bool Channel::isOperator(Client& client)
{
    return _operators.find(&client) != _operators.end();
}

bool Channel::checkmode(char mode)
{
    std::string modeStr(1, mode);

    if (this->_mode.find(modeStr) != this->_mode.end())
	{
		return true;
	}

	return false;
}

Channel::Channel(const std::string &ChannelName, Client &client, std::string key)
    : _isinviteOnly(false), _isTopicRestricted(false), _channelName(ChannelName)
{
    char        buf[100];
    time_t      clock;
    struct tm*	tm_struct;

    if (key != "") {
        this->_password = key;
    }
    else{
        this->_password = "";
    }
    this->_topic = "";
    _clients.insert(&client);
    this->_owner = &client;
    this->_clientAuth[client.getNick()] = "OWNER";
    this->_accessLimit = 1000000;
    this->addOperator(client);
    clock = time(NULL);
    tm_struct = localtime(&clock);
    strftime(buf, 100, "%Y-%m-%d %H:%M:%S", tm_struct);
    std::cout << "time: " << buf << std::endl;
    this->_create_time = buf;
}

Channel::~Channel() {}

int Channel::addClient(Client &client)
{
    _clients.insert(&client);
    return 0;
}

void Channel::addOperator(Client &client)
{
    _operators.insert(&client);
}

void Channel::dismissOperator(Client &client)
{
    _operators.erase(&client);
}

int Channel::removeOperator(Client &client)
{
    _operators.erase(&client);
    return 0;
}

int Channel::kickClient(Client &client)
{
    _clients.erase(&client);
    return 0;
}

std::string Channel::get_Topic_Setter(){
    return this->_topicSetUser;
}

std::string Channel::get_Channel_TopicSetTime(){
    return this->_topicSetTime;
}

void Channel::setOwner(Client& client)
{
	this->_owner = &client;
}

void Channel::joinClient(Client& client, std::string auth)
{
	if (auth == "OWNER")
		this->setOwner(client);
	std::string name = client.getNick();
	this->_clients.insert(&client);
    this->_clientAuth[client.getNick()] = auth;
}

int Channel::setTopic(Client &client, const std::string &topic)
{
    char		buf[100];
	time_t		clock_timer;
    struct tm*	tm_struct;

	clock_timer = time(NULL);
	tm_struct = localtime(&clock_timer);

    _topicSetUser = client.getNick();
	strftime(buf, 100, "%Y-%m-%d %H:%M:%S", tm_struct);
	_topicSetTime = buf;
    _topic = topic;
    return (0);
}

unsigned int Channel::get_User_Limit(){
    return this->_accessLimit;
}

void Channel::setPassword(std::string password){
    this->_password = password;
}

void Channel::setMode(std::string mode)
{
    this->_mode.insert(mode);
}

void Channel::delMode(std::string mode)
{
    this->_mode.erase(mode);
}

void    Channel::setName( std::string name )
{
    this->_channelName = name;
}

std::set<Client *> Channel::retrieve_Clients()
{
    return _clients;
}

Client* Channel::retrieve_Client(std::string nickname)
{
    for (std::set<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        Client* client = *it;
        if (client->getNick() == nickname)
        {
            return client;
        }
    }
    return NULL;
}

std::string Channel::getName(){
    return this->_channelName;
}

void removeChannel(Server *server, const std::string &channelName)
{
    Channel *channel = server->retrieve_Channel(channelName);

    if (channel)
        server->retrieve_Channels().erase(channel->getName());
	else
        std::cout << "Channel not found: " << channelName << std::endl;
}

void Channel::set_User_Limit(unsigned int limit)
{
    this->_accessLimit = limit;
}

std::string Channel::get_Channel_Password()
{
    return this->_password;
}

std::set<std::string> Channel::get_Channel_Mode()
{
    return this->_mode;
}

bool Channel::check_invite_Mode() const
{
	if (this->_mode.find("i") != this->_mode.end())
		return true;
	return false;
}

bool Channel::check_Invitation(std::string nickname)
{
    if (this->_inviteName.find(nickname) == this->_inviteName.end()){
        return false;
    }
    else{
        this->_inviteName.erase(nickname);
        return true;
    }
}

std::string Channel::get_Channel_Topic()
{
    return (this->_topic);
}

bool    Channel::is_ClientIn_Channel( std::string nick )
{
	ClientSet	clients = this->retrieve_Clients();

	for (ClientSet::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (nick == (*it)->getNick())
            return (true);
	}

	return (false);
}

void	Channel::remove_Client_From_Channel(std::string nick )
{
	ClientSet			clients = this->retrieve_Clients();
	ClientSet::iterator	it = clients.begin();

	for (; it != clients.end(); it++)
	{
		if (nick == (*it)->getNick())
		{
			std::cout << "remove client " << (*it)->getNick() << " from " << this->getName() << std::endl;
			clients.erase(it);
            break;
		}
	}

	this->_clients = clients;
}


void	Channel::remove_Client_Authorization( std::string nick )
{
    this->_clientAuth.erase(nick);
}

ClientSet	&Channel::get_Channel_Operators()
{
    return (this->_operators);
}

int Channel::remove_Client_From_Server(Server *server, Client &client )
{
    _clients.erase(&client);
	_operators.erase(&client);
	_inviteName.erase(client.getNick());

    if (_clients.empty())
    {
        removeChannel(server, this->getName());
    }

    return 0;
}

std::string Channel::get_Channel_Creation_Time()
{
    return this->_create_time;
}

bool Channel::check_Channel_Mode(std::string mode)
{
    if (this->_mode.find(mode) != this->_mode.end())
        return true;
    return false;
}

void   Channel::get_Auth(std::string name)
{
    this->_inviteName.insert(name);
}

std::string	Channel::get_Client_Auth(const std::string &nick )
{
	if (this->_clientAuth.find(nick) != this->_clientAuth.end())
		return (this->_clientAuth[nick]);
	else
		return ("");
}

