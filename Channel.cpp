#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel(const std::string& name) : _name(name), _opt_i(false), _opt_t(false), _opt_l(0) {
}

// client가 채널에 있으면 true, 없으면 false 반환
bool Channel::checkChannelMember(Client* client) {
	std::map<Client *, bool>::iterator it = _user_list.find(client);
	if (it == _user_list.end()) { 
		return false;
	}	
	return true;
}

// client가 권한이 있으면 true, 권한이 없으면 false
bool Channel::checkAuthority(Client* client) {
	std::map<Client *, bool>::iterator it = _user_list.find(client);
	return it->second;
}

void	Channel::plusOptI() {
	_opt_i = true;
}

void	Channel::plusOptT() {
	_opt_t = true;
}

void	Channel::plusOptK(const std::string& key) {
	_opt_k = key;
}

void	Channel::plusOptL(std::size_t limit) {
	_opt_l = limit;
}

void	Channel::plusOptO(Client* request_client, Client* target_client) {
	std::map<Client *, bool>::iterator it = _user_list.find(target_client);
	if (it == _user_list.end()) {
		request_client->addMessage(handleResponse(request_client->getNickname(), ERR_USERNOTINCHANNEL, target_client->getNickname(), _name));
		return ;
	}
	it->second = true;
}

void	Channel::minusOptI() {
	_opt_i = false;
}

void	Channel::minusOptT() {
	_opt_t = false;
}

void	Channel::minusOptK() {
	_opt_k = "";
}

void	Channel::minusOptL() {
	_opt_l = 0;
}

void	Channel::minusOptO(Client* request_client, Client* target_client) {
	std::map<Client *, bool>::iterator it = _user_list.find(target_client);
	if (it == _user_list.end()) {
		request_client->addMessage(handleResponse(request_client->getNickname(), ERR_USERNOTINCHANNEL, target_client->getNickname(), _name));
		return ;
	}
	it->second = false;
}

void Channel::join(Client* new_client, const std::string& key) {
	std::vector<std::string>::iterator it = std::find(_invite_list.begin(), _invite_list.end(), new_client->getNickname());
	if (_opt_i == true && it == _invite_list.end()) {
		new_client->addMessage(handleResponse(new_client->getNickname(), ERR_INVITEONLYCHAN, _name));
		return ;
	}
	if (_opt_k != "" && _opt_k != key) {
		new_client->addMessage(handleResponse(new_client->getNickname(), ERR_BADCHANNELKEY, _name));
		return ;
	}
	if (_opt_l != 0 && _user_list.size() >= _opt_l) {
		new_client->addMessage(handleResponse(new_client->getNickname(), ERR_CHANNELISFULL, _name));
		return ;
	}
	if (_opt_i == true && it != _invite_list.end())
		_invite_list.erase(it);
	new_client->addMessage(handleResponse(new_client->getNickname(), RPL_TOPIC, _name, _topic));

	if (_user_list.size() > 0) {
		_user_list.insert(std::make_pair(new_client, false));
	} else {
		_user_list.insert(std::make_pair(new_client, true));
	}
	broadcast(messageFormat(JOIN, new_client, _name));
	std::string temp;
	if (_user_list.begin()->second == true)
		temp = "@" + _user_list.begin()->first->getNickname();
	else
		temp = _user_list.begin()->first->getNickname();
	std::map<Client *, bool>::iterator it2 = ++_user_list.begin();
	for (; it2 != _user_list.end(); it2++) {
		if (it2->second == true)
			temp += " @" + it2->first->getNickname();
		else
			temp += " " + it2->first->getNickname();
	}
	new_client->addMessage(handleResponse(new_client->getNickname(), RPL_NAMREPLY, _name, temp));
	new_client->addMessage(handleResponse(new_client->getNickname(), RPL_ENDOFNAMES, _name));
	new_client->addJoinedChannel(_name);
}

// INVITE
void	Channel::invite(Client* request_client, Client* target_client) {
	if (checkChannelMember(request_client) == false) {
		request_client->addMessage(handleResponse(request_client->getNickname(), ERR_NOTONCHANNEL, _name));
		return ;
	}
	if (checkAuthority(request_client) == false) {
		request_client->addMessage(handleResponse(request_client->getNickname(), ERR_CHANOPRIVSNEEDED, _name));
		return ;
	}
	std::vector<std::string>::iterator it = std::find(_invite_list.begin(), _invite_list.end(), target_client->getNickname());
	if (it == _invite_list.end())
		_invite_list.push_back(target_client->getNickname());
	std::string temp;
	// :<inviter_nick> INVITE <invitee_nick> <channel_name>
	temp = ":" + request_client->getNickname() + " INVITE " + target_client->getNickname() + " " + _name + "\r\n";
	target_client->addMessage(temp);
	broadcast(handleResponse(request_client->getNickname(), RPL_INVITING, _name, target_client->getNickname()));
}

// kick 성공하면 0, 권한이 없으면 1, 없는 client면 2 반환
void	Channel::kick(Client* request_client, Client* target_client, const std::string& reason) {
	if (checkChannelMember(request_client) == false) {
		request_client->addMessage(handleResponse(request_client->getNickname(), ERR_NOTONCHANNEL, _name));
		return ;
	}
	if (checkAuthority(request_client) == false) {
		request_client->addMessage(handleResponse(request_client->getNickname(), ERR_CHANOPRIVSNEEDED, _name));
		return ;
	}
	std::map<Client *, bool>::iterator it = _user_list.find(target_client);
	if (it == _user_list.end()) {
		request_client->addMessage(handleResponse(request_client->getNickname(), ERR_USERNOTINCHANNEL, target_client->getNickname(), _name));
		return ;
	}
	// :<nick>!<user>@<host> KICK <channel> <user> :<comment>
	std::string temp;
	temp = ":" + request_client->getNickname() + "!" + request_client->getUsername() \
				+ "@" + request_client->getHostname() + " KICK " + _name + " " \
				+ target_client->getNickname();
	if (reason != "")
		temp += " :" + reason;
	temp += "\r\n";
	broadcast(temp);
	_user_list.erase(it);
	target_client->deleteJoinedChannel(_name);
}

// topic 설정 성공하면 0, topic-protection mode인데 권한이 없으면 1 반환
void	Channel::topic(Client* request_client, const std::string& topic) {
	if (checkChannelMember(request_client) == false) {
		request_client->addMessage(handleResponse(request_client->getNickname(), ERR_NOTONCHANNEL, _name));
		return ;
	}
	if (_opt_t == true && checkAuthority(request_client) == false) {
		request_client->addMessage(handleResponse(request_client->getNickname(), ERR_CHANOPRIVSNEEDED, _name));
		return ;
	} else {
		// :<nick>!<user>@<host> TOPIC <channel> :<new topic>
		std::string temp;
		temp = ":" + request_client->getNickname() + "!" + request_client->getUsername() \
				+ "@" + request_client->getHostname() + " TOPIC " + _name + " " + \
				topic + "\r\n";
		broadcast(temp);
		_topic = topic;
		return ;
	}
}

// quit을 했을 때 유저를 제거하고 메세지 보냄
void	Channel::quit(Client* request_client, const std::string& message) {
	std::map<Client *, bool>::iterator it = _user_list.find(request_client);
	if (it != _user_list.end()) {
		broadcastWithoutClient(messageFormat(QUIT, request_client, message), request_client);
		_user_list.erase(it);
		deleteInviteList(request_client->getNickname());
	}
}

// 에러로 quit을 했을 때 유저를 제거만 해줌
void	Channel::errorQuit(Client* request_client) {
	std::map<Client *, bool>::iterator it = _user_list.find(request_client);
	if (it != _user_list.end()) {
		_user_list.erase(it);
		deleteInviteList(request_client->getNickname());
	}
}

// 채널의 모든 클라이언트들에게 메세지를 보냄
void	Channel::broadcast(const std::string& message) {
	for (std::map<Client *, bool>::iterator i = _user_list.begin(); i != _user_list.end(); i++)
		i->first->addMessage(message);
}

// client를 제외하고 채널의 모든 클라이언트들에게 메세지를 보냄
void	Channel::broadcastWithoutClient(const std::string& message, Client* client) {
	for (std::map<Client *, bool>::iterator i = _user_list.begin(); i != _user_list.end(); i++)
		if (i->first != client)
			i->first->addMessage(message);
}

// 클라이언트 map getter
const std::map<Client *, bool>	&Channel::getUserList(void) const {
	return _user_list;
}

// name 반환
const std::string&	Channel::getName(void) const {
	return _name;
}

const std::string&	Channel::getTopic(void) const {
	return _topic;
}

void	Channel::answerMode(Client* request_client) {
	std::vector<std::string> answer_vec;
	std::string	answer;

	answer_vec.push_back("+");
	if (_opt_i == true)
		answer_vec[0] += "i";
	if (_opt_t == true)
		answer_vec[0] += "t";
	if (_opt_k != "") {
		answer_vec[0] += "k";
		answer_vec.push_back(_opt_k);
	}
	if (_opt_l != 0) {
		std::stringstream ss;
		answer_vec[0] += "l";
		ss << _opt_l;
		answer_vec.push_back(ss.str());
	}
	answer = answer_vec[0];
	for (std::size_t i = 1; i < answer_vec.size(); i++)
		answer += " " + answer_vec[i];
	request_client->addMessage(handleResponse(request_client->getNickname(), RPL_CHANNELMODEIS, _name, answer));	
}

void Channel::deleteInviteList(std::string del_name) {
	std::vector<std::string>::iterator	it = std::find(_invite_list.begin(), _invite_list.end(), del_name);
	if (it != _invite_list.end())
		_invite_list.erase(it);
}

void Channel::changeInviteNick(const std::string& old_nick, const std::string& new_nick) {
	std::vector<std::string>::iterator it = std::find(_invite_list.begin(), _invite_list.end(), old_nick);
	if (it == _invite_list.end()) 
		return ;
	_invite_list.erase(it);
	_invite_list.push_back(new_nick);
}

void Channel::part(Client* client, const std::string& reason) {
	std::map<Client *, bool>::iterator it = _user_list.find(client);
	if (it == _user_list.end()) {
		client->addMessage(handleResponse(client->getNickname(), ERR_NOTONCHANNEL, _name));
		return ;
	}

	client->deleteJoinedChannel(_name);
	broadcast(messageFormat(PART, client, _name, reason));
	_user_list.erase(it);
}
