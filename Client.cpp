#include "Client.hpp"

Client::Client(int fd)
: _nickname("*"), _fd(fd), _pass(false), _nick(false), _user(false), _allready(false), _disconnect(false) {}

void	Client::setNickname(const std::string& nickname) { _nickname = nickname; }
void	Client::setUsername(const std::string& username) { _username = username; }
void	Client::setRealname(const std::string& realname) { _realname = realname; }
void	Client::setHostname(const std::string& hostname) { _hostname = hostname; }
void	Client::setCommand(const std::string& command) { _command = command; }
void	Client::setServername(const std::string& servername) { _servername = servername; }
void	Client::setPass(bool check) { _pass = check; }
void	Client::setNick(bool check) { _nick = check; }
void	Client::setUser(bool check) { _user = check; }
void	Client::setAllReady(bool check) { _allready = check; }
void	Client::setDisconnect(bool check) { _disconnect = check; }

int	Client::getFd() const { return (_fd); }
const std::string&	Client::getNickname() const { return (_nickname); }
const std::string&	Client::getUsername() const { return (_username); }
const std::string&	Client::getRealname() const { return (_realname); }
const std::string&	Client::getHostname() const { return (_hostname); }
const std::string&	Client::getCommand() const { return (_command); }
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

void Client::addJoinedChannel(const std::string& channel_name) {
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

void	Client::addMessage(const std::string& message) { 
	_message.push_back(message);
}
