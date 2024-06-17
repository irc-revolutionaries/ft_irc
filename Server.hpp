#pragma once

#include "essential.hpp"


class Client;
class Channel;
class Command;

class Server {
public :
	Server	(const char* port, const char* password);
	~Server	();

	const std::map<std::string, Channel *>&	getChannelList() const;
	const std::map<int, Client *>&			getClientList() const;
	const std::string&		getPassword() const;
	std::size_t getFd() const;
	int getPort() const;
	int getKq() const;

	void	setServer();
	void	addClient();
	void	makeCommand(int ident);
	void	sendMessage(int ident);
	void	disconnectClient(int client_fd);
	void	createChannel(std::string ch_name);
	void	deleteChannelList(std::string ch_name);
	void	changeEvents(uintptr_t ident, int16_t filter, \
			uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
	Client*	findClient(const std::string& name);

	void	startServer();
private :
	std::map<std::string, Channel *>	_channel_list;
	std::vector<struct kevent> 			_change_list;
	std::map<int, Client *>				_client_list;
	struct sockaddr_in	_server_addr;
	std::string	_password;
	std::string	_name;
	std::size_t	_fd;
	int		_port;
	int		_kq;


	//Forbidden constructor
	Server	();
	Server	(const Server& copy);
};

void	exitMessage(const std::string& msg);
