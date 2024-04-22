/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akhodara <akhodara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 12:51:53 by ygolshan          #+#    #+#             */
/*   Updated: 2024/03/26 20:25:03 by akhodara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sstream>
#include <cstdint>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <sys/event.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <exception>
#include <algorithm>

#include "Command.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Define.hpp"

class	Command;

class Server
{
	private:
		std::string							_serverName;
		std::string							_hostname;

		int									_port;
		int									_serverSoc;
		std::string							_password;

		int									_kq;
		struct kevent						_eventList[128];
		std::vector<struct kevent>			_changeList;

		std::map<int, Client *>				_clients;
		std::map<std::string, Channel *>	_channels;

		Server();
		Server( const Server &s );

		Server &operator=( const Server &s );

		void		initialize_Server();
		void		register_Event( int socket, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata );


		void		process_Event( struct kevent &kev );

		void		accept_Client();
		void		read_From_Client( struct kevent &kev );
		void		send_Data_To_Client( uintptr_t ident );
		void		remove_Client( uintptr_t ident );

		void		process_Message( Client *client );
		void		process_Command( Client *client, std::string line, std::string buf, size_t crlf );

		void		set_Non_Blocking( int fd );
		void		_free();
		void		_exit( const char *errmsg );


	public:
		Server( int port, std::string password );
		~Server();

		void		run();
		void 		broadcast_Message(std::string &channel_name, const std::string &msg);
		void 		changeEvent(std::vector<struct kevent> &change_list, uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
		void 		update_Channel_Nickname(Client& client, const std::string& before, const std::string& before_prefix);

		bool 		is_ClientIn_Channel(const std::string& nickname);
		bool		isChannel(std::string &ch_name);


		std::string	get_Channel_Password() const;
		std::string add_CRLF(std::string buffer);

		ChannelMap	&retrieve_Channels();
		Channel 	*initialize_Channel(std::string ch_name, std::string key, Client &client);
		Channel		*retrieve_Channel( std::string channelName );
		Client		*retrieve_Client( std::string nick );
};
#endif