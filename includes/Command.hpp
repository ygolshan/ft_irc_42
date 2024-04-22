/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akhodara <akhodara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 12:49:34 by ygolshan          #+#    #+#             */
/*   Updated: 2024/03/26 20:24:57 by akhodara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <sstream>
#include <string>

#include "Server.hpp"
#include "Define.hpp"

class	Client;
class	Server;
class	Channel;

class	Command
{
	private:
		Command();
		Command( const Command &c );
		~Command();

		Command	&operator=( const Command &c );

		void	_split( Client *client, std::istringstream &iss, std::string *split );
		void	transmit_To_Channel( Server *server, Client *client, std::string &target, std::string &message );
		void	transmit_To_Client( Server *server, Client *client, std::string &target, std::string &message );
		void	_removeCRLF( std::string &str );
		void	remove_Client_From_Channel( Server *server, Client *client, Channel *channel );

		std::string execute_Channel_join_Requests(Server *server, Client &client, const std::string &channelName, const std::string &accessKey, std::string &result);
		std::string manage_Client_join_Channel(Server *server, Client &client, std::string &ch_name, std::string &key);

	public:
		static Command	&getInstance();
		void	invite( Server *server, Client *client, std::istringstream &iss );
		void 	join( Server *server, Client *client, std::istringstream &iss );
		void	kick( Server *server, Client *client, std::istringstream &iss );
		void	mode( Server *server, Client *client, std::istringstream &iss );
		void	nick( Server *server, Client *client, std::istringstream &iss );
		void	part( Server *server, Client *client, std::istringstream &iss );
		void	pass( Server *server, Client *client, std::istringstream &iss );
		void	ping( Client *client, std::istringstream &iss );
		void	privmsg( Server *server, Client *client, std::istringstream &iss );
		void	quit( Server *server, Client *client, std::istringstream &iss );
		void	topic( Server *server, Client *client, std::istringstream &iss );
		void	user( Client *client, std::istringstream &iss );
};

#endif