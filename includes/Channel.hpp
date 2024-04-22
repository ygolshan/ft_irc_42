/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akhodara <akhodara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 12:41:25 by ygolshan          #+#    #+#             */
/*   Updated: 2024/03/26 20:24:37 by akhodara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include <map>

#include "Client.hpp"
#include "Define.hpp"
#include "Server.hpp"

class Server;

class Channel
{
private:
    unsigned int _accessLimit;

    bool _isinviteOnly;
    bool _isTopicRestricted;

    std::string _create_time;
    Client      *_owner;
    std::string _channelName;
    std::string _topic;
    std::string _password;
    std::string _topicSetUser;
    std::string _topicSetTime;

    std::set<Client *> _clients;
    std::set<Client *> _operators;

    std::set<std::string> _mode;
    std::set<std::string> _inviteName;

    std::map<std::string, std::string> _clientAuth;

public:
    Channel();
    Channel(const std::string &channelName, Client &client, std::string key);
    Channel(const Channel &c);
    Channel &operator=(const Channel &c);

    bool    operator<( const Channel &c ) const;
	bool	operator<=( const Channel &c ) const;
	bool	operator>( const Channel &c ) const;
	bool	operator>=( const Channel &c ) const;

    ~Channel();

    int addClient(Client &client);
    int kickClient(Client &client);
    int remove_Client_From_Server(Server *server, Client &client);
    int removeOperator(Client &client);
    int setTopic(Client &client, const std::string &topic);
    unsigned int get_User_Limit();
    
    bool check_invite_Mode() const;
    bool check_Invitation(std::string nickname);
    bool isOwner(Client& client);
    bool isOperator(Client& client);
    bool checkmode(char mode);
    bool check_Channel_Mode(std::string mode);

    std::string get_Client_Auth(const std::string &nick );
    void joinClient(Client& client, std::string auth);
    void setOwner(Client& client);
    void setMode(std::string mode);
    void setPassword(std::string password);
    void set_User_Limit(unsigned int limit);
    void delMode(std::string mode);
    void addOperator(Client &client);
    void dismissOperator(Client &client);
    void get_Auth(std::string name);
    std::string get_Channel_Creation_Time();
    void setName( std::string name );

    Client *retrieve_Client(std::string nickname);

    std::string getName();
    std::set<std::string> get_Channel_Mode();
    std::set<Client *> retrieve_Clients();

    std::string get_Channel_Topic();
    std::string get_Channel_Password();
    std::string get_Topic_Setter();
    std::string get_Channel_TopicSetTime();

    bool    is_ClientIn_Channel( std::string nick );

    void    remove_Client_From_Channel(std::string nick );
	void	remove_Client_Authorization( std::string nick );

    ClientSet   &get_Channel_Operators();
};

#endif