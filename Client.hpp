
#pragma once

#include <string>
#include <vector>

class Client{
public :
	Client(const std::string& name, int fd);
	void	sendMessage();
	void	join();
	void	nick();
	void	user();
private :
	std::string _nick_name;
	int			_fd;
	std::vector<std::string> _joined_channel;
};