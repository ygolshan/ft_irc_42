/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Define.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akhodara <akhodara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 12:50:13 by ygolshan          #+#    #+#             */
/*   Updated: 2024/03/26 20:25:01 by akhodara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef DEFINE_HPP
#define DEFINE_HPP

#include <map>
#include <set>
#include <vector>

class Client;
class Channel;

typedef std::map<int, Client *>				ClientMap;
typedef std::map<std::string, Channel *>	ChannelMap;
typedef std::vector<struct kevent>			EventVec;
typedef std::set<Channel *>					ChannelSet;
typedef std::set<Channel *>					invitedSet;
typedef std::set<Client *>					ClientSet;

#define SUCCESS 0
#define FAILURE -1

#define ERROR -1
#define CHANNEL 1
#define NICKNAME 2

#define RET_SUCCESS 0
#define RET_ERROR -1

#define FD_DEFAULT -1
#define FD_ERROR -1

#define MAX_CLIENT 30

#define CRLF "\r\n"

#define RPL_WELCOME(nick)										        	"001 " + nick + ":Welcome to our ircserv " + nick + ". Enjoy your stay!"
#define RPL_YOURHOST(serverName)								        	"002 " + serverName + " :Your host is " + serverName + ", running version 1.0"
#define ERR_NOSUCHNICK(user, nick)									    	"401 " + user + ":No such nick named " + nick
#define ERR_NOSUCHSERVER(user, server)								    	"402 " + user + ":No such server named " + server
#define ERR_NOSUCHCHANNEL(user, channel)								    "403 " + user + ":No such channel named " + channel
#define ERR_NOSUCHCHANNEL2(user, channel)								    "403 " + user + " " + channel + " :No such channel"
#define ERR_NOSUCHCHANNEL3(user, channel)								    "403 " + user + " " + channel + " :you left"
#define ERR_NOTONCHANNEL2(user, channel)								    "442 " + user + " " + channel + " :You're not on that channel!"
#define ERR_CANNOTSENDTOCHAN(user, channel)								    "404 " + user + ":Cannot send to channel " + channel
#define ERR_TOOMANYCHANNELS(user)									    	"405 " + user + ":You have joined too many channels"
#define ERR_NONICKNAMEGIVEN(user)										    "431 " + user + ":Nickname not given"
#define ERR_NICKNAMEINUSE(user, nick)								    	"433 " + user + " " + nick + " :is already in use"
#define ERR_USERNOTINCHANNEL(user, nick, channel)					    	"441 " + user + " " + nick + " " + channel + " :They are not on that channel"
#define ERR_NOTONCHANNEL(user, channel)								    	"442 " + user + " " + channel + " :You are not in the channel"
#define ERR_USERONCHANNEL(user, nick, channel)						    	"443 " + user + " " + nick + " " + channel + " :is already on channel"
#define ERR_NOTREGISTERED(user)										    	"451 " + user + ":You have not registered"
#define ERR_NEEDMOREPARAMS(user, command)							    	"461 " + user + " " + command + " :Need more parameters"
#define ERR_ALREADYREGISTRED(user)									    	"462 " + user + ":You have already registered"
#define ERR_PASSWDMISMATCH(user)									    	"464 " + user + ":Password incorrect "
#define ERR_NOORIGIN(user)											    	"465 " + user + " :No origin specified"
#define ERR_CHANNELISFULL(user, channel)							    	"471 " + user + " " + channel + " :Channel is full"
#define ERR_UNKNOWNMODE(user, mode)									    	"472 " + user + " " + mode + " :unknown mode"
#define ERR_inviteONLYCHAN(user, channel)								    "473 " + user + " " + channel + " :You are not invited"
#define ERR_BADCHANNELKEY(user, channel)								    "475 " + user + " " + channel + " :Wrong channel key"
#define ERR_CHANOPRIVSNEEDED( channel)									    "482 " + channel + " :You are not channel operator"
#define RPL_ENDOFWHO(user, name)											"315 " + user + " " + name + " :End of /WHO list"
#define RPL_LISTSTART(user)													"321 " + user + " Channels :Users Name"
#define RPL_LIST(user, channel, visible, topic)								"322 " + user + " " + channel + " " + visible + " :" + topic
#define RPL_LISTEND(user)													"323 " + user + " :End of /LIST"
#define RPL_CHANNELMODEIS(nick, channel, mode, mode_params)					"324 " + nick + " " + channel + " " + mode + " " + mode_params
#define RPL_CHANNELCREATETIME(nick, channel, date)							"329 " + nick + " " + channel + " :created at " + date
#define RPL_NOTOPIC(user, channel)											"331 " + user + " " + channel + " :Topic not set"
#define RPL_TOPIC(user, channel, topic)										"332 " + user + " " + channel + " :" + topic
#define RPL_TOPICWHOTIME(nick, channel, nick2, setat)						"333 " + nick + " " + channel + " " + nick2 + " " + setat
#define RPL_INVITING(user, nick, channel)									"341 " + user + " " + nick + " " + channel
#define RPL_WHOREPLY(user1, channel, user2, host, server, nick, opt, real)	"352 " + user1 + " " + channel + " " + user2 + " " + host + " " + server + " " + nick + " " + opt + " " + ":0 " + real
#define RPL_NAMREPLY(nick, channel, users)									"353 " + nick + " = " + channel + " :" + users;
#define RPL_ENDOFNAMES(nick, channel)										"366 " + nick + " " + channel + " :End of /NAMES list."
#define RPL_QUIT(nick, message)										        ":" + nick + " QUIT :Quit: " + message
#define ERR_QUIT(user, message)										        "ERROR :Closing link: (" + user + ") [Quit: " + message + "]"
#define RPL_join(nick, channel)										        ":" + nick + " join :" + channel
#define RPL_PRIVMSG(nick, target, msg)								        ":" + nick + " PRIVMSG " + target + msg
#define RPL_TOPIC2(nick, channel, topic)								    ":" + nick + " TOPIC " + channel + " " + topic
#define RPL_PART(user, nick, channel)								        ":" + nick + " PART :" + channel
#define RPL_KICK(nick, channel, nick2, msg)							        ":" + nick + " KICK " + channel + " " + nick2 + " :" + msg
#define RPL_invite(nick, nick2, channel)							        ":" + nick + " invite " + nick2 + " :" + channel
#define RPL_MODE(nick, channel, modes, params)						        ":" + nick + " MODE " + channel + " " + modes + params
#define RPL_NICK(before, after)										        ":" + before + " NICK :" + after
#define RPL_PONG(nick, ping)										        ":" + nick + " PONG :" + ping

#endif