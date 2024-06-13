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

// i 옵션 설정되면 0, 권한이 없으면 1 반환
bool	Channel::plusOptI(Client* request_client) {
	std::cout << "i option called ~~~" << std::endl;
	if (checkChannelMember(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_NOTONCHANNEL, _name));
		return true;
	}
	if (checkAuthority(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_CHANOPRIVSNEEDED, _name));
		return true;
	}
	_opt_i = true;
	return false;
}

// t 옵션 설정되면 0, 권한이 없으면 1 반환
bool	Channel::plusOptT(Client* request_client) {
	std::cout << "t option called ~~~" << std::endl;
	if (checkChannelMember(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_NOTONCHANNEL, _name));
		return true;
	}
	if (checkAuthority(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_CHANOPRIVSNEEDED, _name));
		return true;
	}
	_opt_t = true;
	return false;
}

// k 옵션 설정되면 0, 권한이 없으면 1 반환
bool	Channel::plusOptK(Client* request_client, const std::string& key) {
	std::cout << "+K+K+K+K+K+K" << key << std::endl;
	if (checkChannelMember(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_NOTONCHANNEL, _name));
		return true;
	}
	if (checkAuthority(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_CHANOPRIVSNEEDED, _name));
		return true;
	}
	_opt_k = key;
	return false;
}

// l 옵션 설정되면 0, 권한이 없으면 1 반환, 기존의 limit가 더 크거나 같으면 2, 이미 기준을 초과해서 클라이언트가 있으면 3 반환
bool	Channel::plusOptL(Client* request_client, std::size_t limit) {
	std::cout << "l option called ~~~" << std::endl;
	if (checkChannelMember(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_NOTONCHANNEL, _name));
		return true;
	} else if (checkAuthority(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_CHANOPRIVSNEEDED, _name));
		return true;
	}
	_opt_l = limit;
	return false;
}

// o 옵션 설정되면 0, 권한이 없으면 1 반환, target이 없으면 2 반환
bool	Channel::plusOptO(Client* request_client, Client* target_client) {
	std::cout << "o option called ~~~" << std::endl;
	if (checkChannelMember(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_NOTONCHANNEL, _name));
		return true;
	}
	if (checkAuthority(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_CHANOPRIVSNEEDED, _name));
		return true;
	}
	std::map<Client *, bool>::iterator it = _user_list.find(target_client);
	if (it == _user_list.end()) {
		// request_client->setMessage(handleResponse(request_client->getNickname(), ERR_USERNOTINCHANNEL));
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_USERNOTINCHANNEL, target_client->getNickname(), _name));
		return false;
	}
	it->second = true;
	return false;
}

// i 옵션 제거되면 0, 권한이 없으면 1 반환
bool	Channel::minusOptI(Client* request_client) {
	std::cout << "-i option called ~~~" << std::endl;
	if (checkChannelMember(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_NOTONCHANNEL, _name));
		return true;
	}
	if (checkAuthority(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_CHANOPRIVSNEEDED, _name));
		return true;
	}
	_opt_i = false;
	return false;
}

// t 옵션 제거되면 0, 권한이 없으면 1 반환
bool	Channel::minusOptT(Client* request_client) {
	std::cout << "-t option called ~~~" << std::endl;
	if (checkChannelMember(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_NOTONCHANNEL, _name));
		return true;
	}
	if (checkAuthority(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_CHANOPRIVSNEEDED, _name));
		return true;
	}
	_opt_t = false;
	return false;
}

// k 옵션 제거되면 0, 권한이 없으면 1 반환
bool	Channel::minusOptK(Client* request_client) {
	std::cout << "-k option called ~~~" << std::endl;
	if (checkChannelMember(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_NOTONCHANNEL, _name));
		return true;
	}
	if (checkAuthority(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_CHANOPRIVSNEEDED, _name));
		return true;
	}
	_opt_k = "";
	return false;
}

// l 옵션 제거되면 0, 권한이 없으면 1 반환
bool	Channel::minusOptL(Client* request_client) {
	std::cout << "-l option called ~~~" << std::endl;
	if (checkChannelMember(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_NOTONCHANNEL, _name));
		return true;
	}
	if (checkAuthority(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_CHANOPRIVSNEEDED, _name));
		return true;
	}
	_opt_l = 0;
	return false;
}

// o 옵션 제거되면 0, 권한이 없으면 1 반환, target이 없으면 2 반환
bool	Channel::minusOptO(Client* request_client, Client* target_client) {
	std::cout << "-o option called ~~~" << std::endl;
	if (checkChannelMember(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_NOTONCHANNEL, _name));
		return true;
	}
	if (checkAuthority(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_CHANOPRIVSNEEDED, _name));
		return true;
	}
	std::map<Client *, bool>::iterator it = _user_list.find(target_client);
	if (it == _user_list.end()) {
		// request_client->setMessage(handleResponse(request_client->getNickname(), ERR_USERNOTINCHANNEL));
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_USERNOTINCHANNEL, target_client->getNickname(), _name));
		return false;
	}
	it->second = false;
	return false;
}

// join 성공하면 0, invite-only + invite 못 받았으면 1, key가 틀렸으면 2, 인원수 초과면 3 반환
void Channel::join(Client* new_client, const std::string& key) {
	std::vector<std::string>::iterator it = std::find(_invite_list.begin(), _invite_list.end(), new_client->getNickname());
	if (_opt_i == true && it == _invite_list.end()) {
		new_client->setMessage(handleResponse(new_client->getNickname(), ERR_INVITEONLYCHAN, _name));
		return ;
	}
	if (_opt_k != "" && _opt_k != key) {
		new_client->setMessage(handleResponse(new_client->getNickname(), ERR_BADCHANNELKEY, _name));
		return ;
	}
	if (_opt_l != 0 && _user_list.size() >= _opt_l) {
		new_client->setMessage(handleResponse(new_client->getNickname(), ERR_CHANNELISFULL, _name));
		return ;
	}
	if (_opt_i == true && it != _invite_list.end())
		_invite_list.erase(it);
	new_client->setMessage(handleResponse(new_client->getNickname(), RPL_TOPIC, _name, _topic));

	if (_user_list.size() > 0) {
		_user_list.insert(std::make_pair(new_client, false));
	} else {
		_user_list.insert(std::make_pair(new_client, true));
	}
	broadcast(messageFormat(JOIN, new_client, _name));
	if (_user_list.size() > 0) {
		std::string temp;
		if (_user_list.begin()->second == true)
			temp = "@" + _user_list.begin()->first->getNickname();
		else
			temp = _user_list.begin()->first->getNickname();
		std::map<Client *, bool>::iterator it = _user_list.begin();
		it++;
		for (; it != _user_list.end(); it++) {
			if (it->second == true) 
				temp += " @" + it->first->getNickname();
			else 
				temp += " " + it->first->getNickname();
		}
		new_client->setMessage(handleResponse(new_client->getNickname(), RPL_NAMREPLY, _name, temp));
	} else
		new_client->setMessage(handleResponse(new_client->getNickname(), RPL_NAMREPLY, _name, "@" + new_client->getNickname()));
	new_client->setMessage(handleResponse(new_client->getNickname(), RPL_ENDOFNAMES, _name));
	new_client->setJoinedChannel(_name);
}

// INVITE
void	Channel::invite(Client* request_client, Client* target_client) {
	if (checkChannelMember(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_NOTONCHANNEL, _name));
		return ;
	}
	if (checkAuthority(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_CHANOPRIVSNEEDED, _name));
		return ;
	}
	std::vector<std::string>::iterator it = std::find(_invite_list.begin(), _invite_list.end(), target_client->getNickname());
	if (it != _invite_list.end()) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_USERONCHANNEL, _name));
		return ;
	}
	_invite_list.push_back(target_client->getNickname());
	std::string temp;
	// :<inviter_nick> INVITE <invitee_nick> <channel_name>
	temp = ":" + request_client->getNickname() + " INVITE " + target_client->getNickname() + " " + _name + "\r\n";
	target_client->setMessage(temp);
	broadcast(handleResponse(request_client->getNickname(), RPL_INVITING, _name, target_client->getNickname()));
}

// kick 성공하면 0, 권한이 없으면 1, 없는 client면 2 반환
void	Channel::kick(Client* request_client, Client* target_client, const std::string& reason) {
	// std::cout << "request : " << request_client->getNickname() << "\n target : " << target_client->getNickname() << std::endl;
	if (checkChannelMember(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_NOTONCHANNEL, _name));
		return ;
	}
	if (checkAuthority(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_CHANOPRIVSNEEDED, _name));
		return ;
	}
	std::map<Client *, bool>::iterator it = _user_list.find(target_client);
	if (it == _user_list.end()) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_USERNOTINCHANNEL, target_client->getNickname(), _name));
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
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_NOTONCHANNEL, _name));
		return ;
	}
	if (_opt_t == true && checkAuthority(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_CHANOPRIVSNEEDED, _name));
		return ;
	} else {
		// :<nick>!<user>@<host> TOPIC <channel> :<new topic>
		std::string temp;
		temp = ":" + request_client->getNickname() + "!" + request_client->getUsername() \
				+ "@" + request_client->getHostname() + " TOPIC " + _name + " :" \
				+ topic + "\r\n";
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
	if (it != _user_list.end())
		_user_list.erase(it);
		deleteInviteList(request_client->getNickname());
}

// 채널의 모든 클라이언트들에게 메세지를 보냄
void	Channel::broadcast(const std::string& message) {
	for (std::map<Client *, bool>::iterator i = _user_list.begin(); i != _user_list.end(); i++)
		i->first->setMessage(message);
}

// client를 제외하고 채널의 모든 클라이언트들에게 메세지를 보냄
void	Channel::broadcastWithoutClient(const std::string& message, Client* client) {
	for (std::map<Client *, bool>::iterator i = _user_list.begin(); i != _user_list.end(); i++) {
		if (i->first != client) {
			i->first->setMessage(message);
		}
	}
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
	request_client->setMessage(handleResponse(request_client->getNickname(), RPL_CHANNELMODEIS, _name, answer));	
}

void Channel::deleteInviteList(std::string del_name) {
	std::vector<std::string>::iterator	it = std::find(_invite_list.begin(), _invite_list.end(), del_name);
	if (it != _invite_list.end())
		_invite_list.erase(it);
}
