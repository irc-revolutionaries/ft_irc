
#pragma once

#include "essential.hpp"
#include <string>
#include <vector>

class Client {
//junhylee damdang
public :
	Client(const std::string& name, int fd);
	void setJoinedChannel(const std::string& channel_name);
	void setPass(bool check);
	void setNick(bool check);
	void setUser(bool check);
	void setNickname(const std::string& nickname);
private :
	std::string _nickname;
	int			_fd;
	bool		_pass;
	bool		_nick;
	bool		_user;
	std::vector<std::string> _joined_channel;
};