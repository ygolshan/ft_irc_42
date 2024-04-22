/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akhodara <akhodara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 12:42:09 by ygolshan          #+#    #+#             */
/*   Updated: 2024/03/26 20:24:49 by akhodara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Command.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Define.hpp"
#include "Server.hpp"

#include <iostream>
#include <unistd.h>
#include <string>
#include <set>
#include <map>

class Channel;


class	Client
{
	private:
		static int	_clientNum;
		bool		_isRegistered;
		bool		_isClosed;
		int			_clientSoc;
		std::string	_addr;

		std::string	_nick;
		std::string	_userName;
		std::string	_realName;
		std::string	_hostName;
		std::string	_serverName;

		std::string	_buf; 
		std::string	_sendData;

		std::map<std::string, Channel *>	_joinedChannels;
		std::set<Channel *>	_invited;


	public:
		Client( int socket, std::string addr );
		Client();
		Client( const Client &c );

		Client	&operator=( const Client &c );
		~Client();

		bool	isRegistered() const;
		bool	isClosed() const;
		void	setRegistered( bool isRegistered );
		void	setClosed( bool isClosed );

		int			getSocket() const;
		std::string	getAddr() const;
		std::string	getNick() const;
		void		setNick( const std::string &nick );
		std::string	getUserName() const;
		void		setUserName( const std::string &userName );
		std::string	getRealName() const;
		void		setRealName( const std::string &realName );
		std::string	getHostName() const;
		void		setHostName( const std::string &hostName );
		std::string	getServerName() const;
		void		setServerName( const std::string &serverName );
		std::string	getBuf() const;
		void		setBuf( std::string &buf );
		void		setBuf( std::string buf );
		void		clearBuf();
		void		append_To_Buffer( std::string buf );
		void		setSendData( std::string sendData );
		std::string	getSendData() const;
		void		append_To_Send_Data( std::string sendData );
		void		clear_Send_Data();

		void		disconnect_From_Channels(Server *server);
		
		std::string get_Client_Prefix();
		ChannelMap	retrieve_Channels();
		void 		join_Channel(Channel *channel);

		ChannelMap	&get_joined_Channels();
		Channel		*find_joined_Channel( std::string &channelName );
		void		remove_joined_Channel( std::string &channelName );

		ChannelSet	&get_invited_Channels();
		void		add_invited_Channel( Channel *channel );
};

#endif