#include "Client.hpp"

Client::Client(const std::string& nickname, int fd)
: _nickname(nickname), _fd(fd), _pass(0), _nick(0), _user(0) {}

void Client::setJoinedChannel(const std::string& channel_name) {
	this->_joined_channel.push_back(channel_name);
}

void Client::setPass(bool check) { this->_pass = check; }
void Client::setNick(bool check) { this->_nick = check; }
void Client::setUser(bool check) { this->_user = check; }
void Client::setNickname(const std::string& nickname) { this->_nickname = nickname; }