#include "Client.hpp"

Client::Client(int fd)
: _fd(fd), _pass(false), _nick(false), _user(false), _allready(false), _disconnect(false) {}

void	Client::setNickname(const std::string& nickname) { this->_nickname = nickname; }
void	Client::setUsername(const std::string& username) { this->_username = username; }
void	Client::setRealname(const std::string& realname) { this->_realname = realname; }
void	Client::setHostname(const std::string& hostname) { this->_hostname = hostname; }
void	Client::setServername(const std::string& servername) { this->_servername = servername; }
void	Client::setMessage(const std::string& message) { this->_message.push_back(message); }
void	Client::setPass(bool check) { this->_pass = check; }
void	Client::setNick(bool check) { this->_nick = check; }
void	Client::setUser(bool check) { this->_user = check; }
void	Client::setAllReady(bool check) { this->_allready = check; }
void	Client::setDisconnect(bool check) { this->_disconnect = check; }

int	Client::getFd() const { return (_fd); }
const std::string&	Client::getNickname() const { return (_nickname); }
const std::string&	Client::getUsername() const { return (_username); }
const std::string&	Client::getRealname() const { return (_realname); }
const std::string&	Client::getHostname() const { return (_hostname); }
const std::string&	Client::getServername() const { return (_servername); }
const std::vector<std::string>&	Client::getMessage() const { return (_message); }
bool	Client::getPass() const { return (_pass); }
bool	Client::getNick() const { return (_nick); }
bool	Client::getUser() const { return (_user); }
bool	Client::getAllReady() const { return (_allready); }
bool	Client::getDisconnect() const { return (_disconnect); }

void Client::clearMessage() {
	_message.clear();
}

void Client::setJoinedChannel(const std::string& channel_name) {
	this->_joined_channel.push_back(channel_name);
}

const std::vector<std::string>&	Client::getJoinedChannel() const { return (_joined_channel); }

void Client::deleteJoinedChannel(const std::string& channel_name) {
	std::vector<std::string>::iterator it = find(_joined_channel.begin(), _joined_channel.end(), channel_name);
	if (it != _joined_channel.end())
		_joined_channel.erase(it);
}

void	Client::clearJoinedChannel() {
	_joined_channel.clear();
}
