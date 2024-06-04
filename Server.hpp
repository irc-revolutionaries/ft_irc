#pragma once

#include "essential.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class Client;
class Channel;

class Server {
public :
	Server	(const char* port, const char* password);

	const std::map<std::string, Channel *>&	getChannelList() const;
	const std::map<int, Client *>&			getClientList() const;
	const std::string&		getPassword() const;
	const int getPort() const;
	const int getFd() const;
	const int getKq() const;

	void	setServer(std::vector<struct kevent>& change_list);
	void	addClient(std::vector<struct kevent>& change_list);
	void	disconnectClient(int client_fd);
	void	createChannel(Client* first_client, std::string ch_name);
	Client*	findClient(const std::string& name) const;
private :
	std::map<std::string, Channel *>	_channel_list;
	std::map<std::string, Client *>		_client_list;
	struct sockaddr_in	_server_addr;
	std::string	_password;
	int	_port;
	int	_fd;
	int	_kq;
};