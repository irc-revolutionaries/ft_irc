
#pragma once

#include <vector>
#include <map>

class Client;
class Channel;

class Server{
//mkong damdang
public :
	// 서버 생성자에서 kqueue
	Server();
	void	startServer();
	void	addClient();
	void	handleMessage();
	void	joinChannel();
private :
	int	_fd;
	std::string port;
	std::string password;
	std::map<std::string, Client *>	_client_list;
	std::map<std::string, Channel *> _channel_list;//map으로 갖고있는게 더 편할듯
};