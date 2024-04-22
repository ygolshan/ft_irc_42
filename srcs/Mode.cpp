/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akhodara <akhodara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 12:46:04 by ygolshan          #+#    #+#             */
/*   Updated: 2024/03/26 20:25:34 by akhodara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Command.hpp"
#include "../includes/Client.hpp"
#include "../includes/Define.hpp"
#include "../includes/Channel.hpp"

void Command::mode(Server *server, Client *client, std::istringstream &iss){
    std::string channel_name;
    std::string channel_mode;
    iss >> channel_name;
    iss >> channel_mode;


    if (((channel_name[0] != '#') && (channel_name[0] != '&'))){
        return ;
    }

	if ((server->retrieve_Channels().find(channel_name) == server->retrieve_Channels().end()))
    {
        client->setSendData(ERR_NOSUCHCHANNEL2(client->getNick(), channel_name) + CRLF);
        return ;
    }

    Channel *ch = server->retrieve_Channel(channel_name);

    if (channel_mode.empty()) {
        std::string reply;
        std::string response;
        std::string channel_prefix = "+";
        std::string channel_parameters = "";
        std::string channel_name = ch->getName();

        int key_mode_count = 0;
        int key_mode_added = 0;

        if (ch->check_Channel_Mode("k"))
            key_mode_added++;
        if (ch->check_Channel_Mode("l"))
            key_mode_added++;
        
        std::set<std::string> channel_modes = ch->get_Channel_Mode();

        if (key_mode_added == 0)
            channel_prefix = ":+";
        for (std::set<std::string>::iterator mode_it = channel_modes.begin(); mode_it != channel_modes.end(); mode_it++) {
            if (*mode_it == "k") {
                if (key_mode_count == key_mode_added - 1)
                    channel_parameters += " :";
                else
                    channel_parameters += " ";
                channel_parameters += ch->get_Channel_Password();
                key_mode_count++;
            }
            else if (*mode_it == "l") {
                if (key_mode_count == key_mode_added - 1)
                    channel_parameters += " :";
                else
                    channel_parameters += " ";
                channel_parameters += ch->get_User_Limit();
                key_mode_count++;
            }
            channel_prefix += *mode_it;
        }

        response = RPL_CHANNELMODEIS(client->getNick(), channel_name, channel_prefix, channel_parameters) + CRLF;
        response += RPL_CHANNELCREATETIME(client->getNick(), channel_name, ch->get_Channel_Creation_Time()) + CRLF;

        server->broadcast_Message(channel_name, response);
        client->setSendData(response);
        return;
    }


    int plus_minus = 1;
    int pre_plus_minu = 0;
    std::string mode_msg = "";
    if ((ch->isOwner(*client) || ch->isOperator(*client))){
        plus_minus = 1;
    }
    else {
        std::cout <<  "Error\n";
        client->setSendData(ERR_CHANOPRIVSNEEDED(channel_name) + CRLF);
        return ;
    }
    for (unsigned long i = 0; i < channel_mode.length(); i++){
        if (channel_mode[i] == '+' || channel_mode[i] == '-'){
            if (channel_mode[i + 1] == '+' || channel_mode[i + 1] == '-' 
                || channel_mode[i + 1] == ' '){
                std::cout <<  "Error\n";
                client->setSendData("Error\r\n");
                return ;
            }
            if (channel_mode[i] == '+') plus_minus = 1;
            else plus_minus = -1;
            mode_msg += channel_mode[i];
        }
        else if (channel_mode[i] == 'i'){
            if (plus_minus == 1){
                mode_msg += channel_mode[i];
                if (ch->checkmode(channel_mode[i]))
                    continue;
                std::string tmp_mode(1, channel_mode[i]);
                ch->setMode(tmp_mode);
                pre_plus_minu = 1;
            }
            else {
                mode_msg += channel_mode[i];
                if (!ch->checkmode(channel_mode[i])) continue;
                std::string tmp_mode(1, channel_mode[i]);
                ch->delMode(tmp_mode);
                pre_plus_minu = -1;
            }
            
        }

        else if (channel_mode[i] == 't'){
            if (plus_minus == 1){
                mode_msg += channel_mode[i];
                if (ch->checkmode(channel_mode[i])) continue;
                std::string tmp_mode(1, channel_mode[i]);
                ch->setMode(tmp_mode);
                pre_plus_minu = 1;
            }
            else {
                mode_msg += channel_mode[i];
                if (!ch->checkmode(channel_mode[i])) continue;
                std::string tmp_mode(1, channel_mode[i]);
                ch->delMode(tmp_mode);
                pre_plus_minu = -1;
            }
            
        }
        else if (channel_mode[i] == 'k'){
            std::string new_password;
            iss >> new_password;

            std::cout << "new_password: " << new_password << "\n";

            if (plus_minus == 1){
                if (new_password.empty()){
                    std::cout << "Error\n";
                    client->setSendData(ERR_NEEDMOREPARAMS(client->getNick(), std::string("MODE +k")) + CRLF);
                    return ;
                }
                if (new_password.length() > 20){
                    std::cout << "Error\n";
                    client->setSendData("Error\r\n");
                    return ;
                }
                mode_msg += channel_mode[i];
                std::string tmp_mode(1, channel_mode[i]);
                ch->setPassword(new_password);
                ch->setMode(tmp_mode);
                pre_plus_minu = 1;
            }
            else {
                mode_msg += channel_mode[i];
                if (!ch->checkmode(channel_mode[i])) continue;
                std::string tmp_mode(1, channel_mode[i]);
                new_password = "";
                ch->setPassword(new_password);
                ch->delMode(tmp_mode);
                pre_plus_minu = -1;
            }
        }
        else if (channel_mode[i] == 'o') {
            std::string new_operator_name;
            iss >> new_operator_name;

            std::cout << "new_operator_name: " << new_operator_name << "\n";

            if (new_operator_name.empty()) {
                std::cout << "Error\n";
                client->setSendData(ERR_NEEDMOREPARAMS(client->getNick(), std::string("MODE +o")) + CRLF);
                continue;
            }

            Client* new_operator = ch->retrieve_Client(new_operator_name);

            if (new_operator == NULL) {
                std::cout << "Error\n";
                client->setSendData(ERR_USERNOTINCHANNEL(client->getNick(), new_operator_name, channel_name) + CRLF);
                continue;
            }

            if (plus_minus == 1) {
                mode_msg += channel_mode[i];
                if (ch->isOperator(*new_operator))
                    continue;
                ch->addOperator(*new_operator);
                pre_plus_minu = 1;
            } else {
                mode_msg += channel_mode[i];
                if (!ch->isOperator(*new_operator))
                    continue;
                ch->dismissOperator(*new_operator);
                pre_plus_minu = -1;
            }
        }

        else if (channel_mode[i] == 'l'){
            std::string limit;
            iss >> limit;
            if (plus_minus == 1){
                std::cout <<  "mode l channel name: " <<  ch->getName() << "\n";
                std::cout <<  "mode l channel name: " <<  channel_name  << "\n";
                if (limit.empty())
                {
                    std::cout << "Error\n";
                    client->setSendData(ERR_NEEDMOREPARAMS(client->getNick(), std::string("MODE +l")) + CRLF);
                    continue ;
                }
                if (limit.length() > 10)
                    limit = "0";

                unsigned int LimitNumber = atol(limit.c_str());
                if (LimitNumber < ch->retrieve_Clients().size()){
                    std::cout << "Error\n";
                    client->setSendData("Error\r\n");
                    continue;
                }
                if (ch->checkmode(channel_mode[i]) && LimitNumber < ch->get_User_Limit()){
                    std::cout << "Error\n";
                    client->setSendData("Error\r\n");
                    continue;
                }
                mode_msg += channel_mode[i];
                ch->set_User_Limit(LimitNumber);
                std::string tmp(1, channel_mode[i]);
                ch->setMode(tmp);
                pre_plus_minu = 1;
            }
            else {
                if (!ch->checkmode(channel_mode[i])){
                    std::cout << "Error\n";
                    client->setSendData("Error\r\n");
                    continue;
                }
                mode_msg += channel_mode[i];
                ch->set_User_Limit(10000000);
                std::string tmp(1, channel_mode[i]);
                ch->delMode(tmp);
                pre_plus_minu = 1;
            }
        }

    }
    if (mode_msg.empty())
        mode_msg.insert(0, ":");
    std::cout << "mode_msg: " << mode_msg << "\n";
    std::cout << "length: " << mode_msg.length() << "\n";
    if (!mode_msg.empty() && (mode_msg[0] != '+' || mode_msg[0] != '-') && mode_msg.length() != 1){
        std::string msg = ":" + client->getNick() + " MODE " + channel_name + " " + mode_msg;
        server->broadcast_Message(channel_name, msg);
    } 
}