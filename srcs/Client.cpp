/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akhodara <akhodara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 12:41:52 by ygolshan          #+#    #+#             */
/*   Updated: 2024/03/26 20:25:16 by akhodara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Command.hpp"
#include "../includes/Client.hpp"
#include "../includes/Define.hpp"
#include "../includes/Channel.hpp"

int	Client::_clientNum = 1;

Client::Client() {}

Client::Client( const Client &c )
{
	(void)c;
}

Client::Client( int socket, std::string addr )
	: _isRegistered(false), _isClosed(false), _clientSoc(socket), _addr(addr)
{
	this->_nick = "client" + std::to_string(Client::_clientNum);
	Client::_clientNum++;
}

Client::~Client()
{
	close(this->_clientSoc);
}

Client	&Client::operator=( const Client &c )
{
	(void)c;
	return (*this);
}

bool	Client::isRegistered() const
{
	return (this->_isRegistered);
}

bool	Client::isClosed() const
{
	return (this->_isClosed);
}

void	Client::setRegistered( bool isRegistered )
{
	this->_isRegistered = isRegistered;
}

void	Client::setClosed( bool isClosed )
{
	this->_isClosed = isClosed;
}

int	Client::getSocket() const
{
	return (this->_clientSoc);
}

std::string	Client::getAddr() const
{
	return (this->_addr);
}

std::string	Client::getNick() const
{
	return (this->_nick);
}

std::string	Client::getUserName() const
{
	return (this->_userName);
}

std::string	Client::getRealName() const
{
	return (this->_realName);
}

std::string	Client::getHostName() const
{
	return (this->_hostName);
}

std::string	Client::getServerName() const
{
	return (this->_serverName);
}

std::string	Client::getBuf() const
{
	return (this->_buf);
}

std::string	Client::getSendData() const
{
	return (this->_sendData);
}

void	Client::setNick( const std::string &nick )
{
	this->_nick = nick;
}

void	Client::setUserName( const std::string &userName )
{
	this->_userName = userName;
}

void	Client::setRealName( const std::string &realName )
{
	this->_realName = realName;
}

void	Client::setHostName( const std::string &hostName )
{
	this->_hostName = hostName;
}

void	Client::setServerName( const std::string &serverName )
{
	this->_serverName = serverName;
}

void	Client::setBuf( std::string &buf )
{
	this->_buf = buf;
}

void	Client::setBuf( std::string buf )
{
	this->_buf = buf;
}

void	Client::setSendData( std::string sendData )
{
	this->_sendData = sendData;
}

void	Client::clearBuf()
{
	this->_buf.clear();
}

void	Client::clear_Send_Data()
{
	this->_sendData.clear();
}

void	Client::append_To_Buffer( std::string buf )
{
	this->_buf.append(buf);
}

void	Client::append_To_Send_Data( std::string sendData )
{
	this->_sendData.append(sendData);
}

std::map<std::string, Channel *> Client::retrieve_Channels()
{
	return this->_joinedChannels;
}

std::string Client::get_Client_Prefix()
{
    std::string username = "!" + this->_userName;
    std::string hostname = "@" + this->_hostName;

    return this->_nick + username + hostname;
}

void Client::join_Channel( Channel *channel )
{
	this->_joinedChannels[channel->getName()] = channel;
}

void	Client::disconnect_From_Channels(Server *server)
{
	for (ChannelMap::iterator it = this->_joinedChannels.begin();
	 		it != this->_joinedChannels.end(); it++)
	{
	 	(it)->second->remove_Client_From_Server(server, *this);
	}

	this->_joinedChannels.clear();
	this->_invited.clear();
	this->_buf.clear();
	this->_sendData.clear();
}

ChannelMap	&Client::get_joined_Channels()
{
	return (this->_joinedChannels);
}

Channel	*Client::find_joined_Channel( std::string &channelName )
{
	return (this->_joinedChannels[channelName]);
}

ChannelSet	&Client::get_invited_Channels()
{
	return (this->_invited);
}

void	Client::add_invited_Channel( Channel *channel )
{
	this->_invited.insert(channel);
}

void	Client::remove_joined_Channel( std::string &channelName )
{
	this->_joinedChannels.erase(channelName);
}