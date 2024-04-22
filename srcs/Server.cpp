/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akhodara <akhodara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 12:51:32 by ygolshan          #+#    #+#             */
/*   Updated: 2024/03/26 20:25:38 by akhodara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Command.hpp"
#include "../includes/Client.hpp"
#include "../includes/Define.hpp"
#include "../includes/Channel.hpp"

Server::Server() {}

Server::Server( const Server &s )
{
	(void)s;
}

Server::Server( int port, std::string password )
	: _serverName("ircserv"), _port(port), _serverSoc(FD_DEFAULT), _password(password), _kq(FD_DEFAULT)
{
	(void)_port;
	(void)_password;
}

Server::~Server()
{
	ChannelMap::iterator	it = this->_channels.begin();
	for (; it != this->_channels.end(); it++)
	{
		delete it->second;
		it->second = 0;
	}
	ClientMap::iterator	it2 = this->_clients.begin();
	for (; it2 != this->_clients.end(); it2++)
	{
		delete it2->second;
		it2->second = 0;
	}
}

Server	&Server::operator=( const Server &s )
{
	(void)s;
	return (*this);
}

void	Server::register_Event( int socket, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata )
{
	struct kevent	kev;
	EV_SET(&kev, socket, filter, flags, fflags, data, udata);
	this->_changeList.push_back(kev);
}

void	Server::initialize_Server()
{
	this->_serverSoc = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_serverSoc == -1)
		throw std::runtime_error("Exception: socket error");

	int	opt = 1;
	if (setsockopt(this->_serverSoc, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw std::runtime_error("Exception: socket error");
	struct	sockaddr_in	sockAddr;
	
	bzero(&sockAddr, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(this->_port);
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(this->_serverSoc, (struct sockaddr *)&sockAddr, sizeof(sockAddr)))
	{
		close(this->_serverSoc);
		throw std::runtime_error("Exception: bind error");
	}

	if (listen(this->_serverSoc, 10))
	{
		close(this->_serverSoc);
		throw std::runtime_error("Exception: listen error");
	}

	this->set_Non_Blocking(this->_serverSoc);

	this->_kq = kqueue();
	if (this->_kq == -1)
	{
		close(this->_serverSoc);
		throw std::runtime_error("Exception: kqueue error");
	}
	this->register_Event(this->_serverSoc, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
}

void	Server::run()
{
	this->initialize_Server();

	int				newEv;

	while (1)
	{
		newEv = kevent(this->_kq,
				&this->_changeList[0], this->_changeList.size(),
				this->_eventList, 128, NULL);

		if (newEv == -1)
			this->_exit("Exception: kevent error");

		this->_changeList.clear();

		for (int i = 0; i < newEv; i++)
			this->process_Event(this->_eventList[i]);
	}
}

void	Server::process_Event( struct kevent &kev )
{
	(void)kev;
	if (kev.flags & EV_ERROR)
	{

		if (kev.ident == (uintptr_t)this->_serverSoc)
		{
			this->_free();
			throw std::runtime_error("Exception: server error");
		}
		else
		{
			std::cerr << "Exception: Client error" << std::endl;
			this->remove_Client(kev.ident);
		}
	}
	else if (kev.filter == EVFILT_READ)
	{
		if (kev.ident == (uintptr_t)this->_serverSoc)
			this->accept_Client();
		else
			this->read_From_Client(kev);
	}
	else if (kev.filter == EVFILT_WRITE)
		this->send_Data_To_Client(kev.ident);
}

void	Server::remove_Client( uintptr_t ident )
{
	ClientMap::iterator	it = this->_clients.find(ident);

	if (it == this->_clients.end())
		return ;

	Client	*client = it->second;
	
	this->_clients.erase(ident);

	client->disconnect_From_Channels(this);

	delete client;

	std::cout << "SERVER Client disconnected" << std::endl;
}

void	Server::accept_Client()
{
	int					clientSoc;
	struct sockaddr_in	clientAddr;
	socklen_t			clientAddrSize = sizeof(clientAddr);

	bzero(&clientAddr, sizeof(clientAddr));

	clientSoc = accept(this->_serverSoc, (struct sockaddr *)&clientAddr, &clientAddrSize);
	if (clientSoc == RET_ERROR)
	{
		std::cerr << "Error: accept error" << std::endl;
		return ;
	}

	if (clientSoc >= MAX_CLIENT)
	{
		std::cout << "SERVER Has max Client" << std::endl;
		close(clientSoc);
		return ;
	}
	this->set_Non_Blocking(clientSoc);
	this->register_Event(clientSoc, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	this->register_Event(clientSoc, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	std::string	addr(inet_ntoa(clientAddr.sin_addr));
	Client *newClient = new Client(clientSoc, addr);
	this->_clients.insert(std::make_pair(clientSoc, newClient));

	std::cout << "SERVER New Client connected: " << newClient->getSocket() << std::endl;
}

void	Server::read_From_Client( struct kevent &kev )
{
	char				buf[1024];
	ssize_t				byte;
	ClientMap::iterator	it = this->_clients.find(kev.ident);
	Client				*client = it->second;

	if (it == this->_clients.end()) return ;

	bzero(buf, sizeof(buf));
	byte = recv(kev.ident, buf, sizeof(buf), 0);
	if (byte <= 0)
	{
		if (byte < ERROR)
			return ;
		std::cerr << "< Client " << kev.ident << " > Error: recv error" << std::endl;
		this->remove_Client(kev.ident);
	}
	else
	{
		buf[byte] = '\0';
		client->append_To_Buffer(buf);
		this->process_Message(client);
	}
}

void	Server::process_Message( Client *client )
{
	size_t		crlf;
	std::string	buf;
	std::string	line;

	while (true)
	{
		if (client->isClosed())
			break ;

		buf = client->getBuf();
		crlf = buf.find(CRLF);

		if (crlf != std::string::npos)
		{
			line = buf.substr(0, crlf + 2);
			std::cout << "SERVER message recieved from " << client->getSocket() << ": " << line << std::endl;
			this->process_Command(client, line, buf, crlf);
		}
		else
			break ;
	}
}

void	Server::process_Command( Client *client, std::string line, std::string buf, size_t crlf )
{
	std::string			cmd;
	std::string			msg;
	std::istringstream	ss(line);
	Command				&command = Command::getInstance();

	ss >> cmd;

	for (std::string::iterator it = cmd.begin(); it != cmd.end(); it++)
		*it = toupper(*it);

	if (cmd == "PASS")
		command.pass(this, client, ss);
	else if (client->isRegistered() == false)
		client->setSendData(ERR_NOTREGISTERED(client->getNick()) + std::string(CRLF));
	else if (cmd == "NICK")
		command.nick(this, client, ss);
	else if (cmd == "JOIN")
		command.join(this, client, ss);
	else if (cmd == "USER")
		command.user(client, ss);
	else if (cmd == "PRIVMSG")
		command.privmsg(this, client, ss);
	else if (cmd == "INVITE")
		command.invite(this, client, ss);
	else if (cmd == "MODE")
		command.mode(this, client, ss);
	else if (cmd == "KICK")
		command.kick(this, client, ss);
	else if (cmd == "TOPIC")
		command.topic(this, client, ss);
	else if (cmd == "PART")
		command.part(this, client, ss);
	else if (cmd == "PING")
		command.ping(client, ss);
	else if (cmd == "QUIT")
		command.quit(this, client, ss);
	else if (cmd == "TERM")
		this->_exit("Server terminated");

	client->setBuf(buf.substr(crlf + 2));
}


bool Server::is_ClientIn_Channel(const std::string& nickname)
{
    ClientMap::iterator it = this->_clients.begin();

	for (; it != this->_clients.end(); it++){
		if (it->second->getNick() == nickname)
			return true;
	}
	return false;
}

void Server::update_Channel_Nickname(Client& client, const std::string& before, const std::string& before_prefix)
{
	std::string	auth;
	std::string	channelName;
	ChannelMap channels = client.retrieve_Channels();

	for (ChannelMap::iterator it = channels.begin(); it != channels.end(); it++)
	{
		channelName = it->second->getName();
		auth = (this->_channels[channelName])->get_Client_Auth(before);

		if (auth != "")
		{
			(this->_channels[channelName])->remove_Client_From_Channel(before);
			(this->_channels[channelName])->joinClient(client, auth);

			std::string	res;
			res += ":" + before_prefix + " NICK :" + client.getNick();

			this->broadcast_Message(channelName, res);
		}
	}
}

void	Server::send_Data_To_Client( uintptr_t ident )
{
	int					byte;
	std::string			data;
	ClientMap::iterator	it = this->_clients.find(ident);

	if (it == this->_clients.end()) return ;

	Client	*client = it->second;
	if (client->getSendData().empty()) return ;

	std::cout << "SERVER message sent: " << client->getSendData() << std::endl;

	data = client->getSendData();
	byte = send(ident, data.c_str(), data.size(), 0);
	if (byte == RET_ERROR)
	{
		client->clear_Send_Data();
		std::cerr << "Error" << std::endl;
		this->remove_Client(ident);
	}
	else
	{
		client->clear_Send_Data();
		if (client->isClosed())
			this->remove_Client(ident);
	}
}

std::string	Server::get_Channel_Password() const
{
	return (this->_password);
}

bool Server::isChannel(std::string &ch_name){
	if (this->_channels.find(ch_name) != this->_channels.end())
		return true;
	else
		return false;
}

Channel *Server::initialize_Channel( std::string ch_name, std::string key, Client &client)
{
	Channel *newchannel = new Channel(ch_name, client, key);
	this->_channels[ch_name] = newchannel;
	newchannel->setName(ch_name);
	newchannel->addOperator(client);
	std::cout << "create channel: " << ch_name << std::endl;
	return newchannel;
}

std::string Server::add_CRLF(std::string buffer)
{
	return buffer += "\r\n";
}

void Server::changeEvent(std::vector<struct kevent> &change_list, uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
	(void)change_list;
	struct kevent temp_event;
	EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
	this->_changeList.push_back(temp_event);
}

void Server::broadcast_Message(std::string &channel_name, const std::string &msg)
{
	Channel *channel = this->_channels[channel_name];

	std::set<Client *> users = channel->retrieve_Clients();
	for (std::set<Client *>::iterator u_it = users.begin(); u_it != users.end(); ++u_it)
	{
		int clientSoc = (*u_it)->getSocket();
		(*u_it)->setSendData(msg + CRLF);
		changeEvent(_changeList, clientSoc, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
		std::cout << "< Client " << clientSoc << " >" << std::endl;
	}
}

ChannelMap	&Server::retrieve_Channels()
{
	return (this->_channels);
}

Channel	*Server::retrieve_Channel( std::string channelName )
{
	ChannelMap::iterator	it;

	for (it = this->_channels.begin(); it != this->_channels.end(); it++)
	{
		if (it->first == channelName)
			return (it->second);
	}

	return (NULL);
}

Client	*Server::retrieve_Client( std::string nick )
{
	ClientMap::iterator	it = this->_clients.begin();

	for (; it != this->_clients.end(); it++)
	{
		if (it->second->getNick() == nick)
			return (it->second);
	}

	return (NULL);
}