
#pragma once

#include <vector>
#include <map>
#include <string>

class Client;

class Channel{//각 채널 마다 유저 리스트, 채널 이름
public :
	Channel(const std::string& name);
	void	addClient();
	void	invite();
	void	kick();
	void	topic();
	void	mode();
	// void	removeClient();
private :
	std::string					_name;
	std::map<Client *, bool>	_user_list;
};
// std::vector<std::map<int, std::string>> _user_list;//map< client_fd, cliend_name >
// std::map<int, bool> _user_operator;//map< client_fd, operator >