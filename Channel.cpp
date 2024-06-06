#include "essential.hpp"
#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel(const std::string& name, Client* first_client) : _name(name), _opt_i(false), _opt_t(false), _opt_l(0) {
	_user_list.insert(std::pair<Client *, bool>(first_client, true));
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
int	Channel::plusOptI(Client* request_client) {
	if (checkAuthority(request_client) == false)
		return 1;
	_opt_i = true;
	return 0;
}

// t 옵션 설정되면 0, 권한이 없으면 1 반환
int	Channel::plusOptT(Client* request_client) {
	if (checkAuthority(request_client) == false)
		return 1;
	_opt_t = true;
	return 0;
}

// k 옵션 설정되면 0, 권한이 없으면 1 반환
int	Channel::plusOptK(Client* request_client, const std::string& key) {
	if (checkAuthority(request_client) == false)
		return 1;
	_opt_k = key;
	return 0;
}

// l 옵션 설정되면 0, 권한이 없으면 1 반환, 기존의 limit가 더 크거나 같으면 2, 이미 기준을 초과해서 클라이언트가 있으면 3 반환
int	Channel::plusOptL(Client* request_client, int limit) {
	if (checkAuthority(request_client) == false) {
		return 1;
	} else if (_opt_l >= limit) {
		return 2;
	} else if (_user_list.size() > limit) {
		return 3;
	}
	_opt_l = limit;
	return 0;
}

// o 옵션 설정되면 0, 권한이 없으면 1 반환, target이 없으면 2 반환
int	Channel::plusOptO(Client* request_client, Client* target_client) {
	if (checkAuthority(request_client) == false)
		return 1;
	std::map<Client *, bool>::iterator it = _user_list.find(target_client);
	if (it == _user_list.end()) {
		return 2;
	}
	it->second = true;
	return 0;
}

// i 옵션 제거되면 0, 권한이 없으면 1 반환
int	Channel::minusOptI(Client* request_client) {
	if (checkAuthority(request_client) == false)
		return 1;
	_opt_i = false;
	return 0;
}

// t 옵션 제거되면 0, 권한이 없으면 1 반환
int	Channel::minusOptT(Client* request_client) {
	if (checkAuthority(request_client) == false)
		return 1;
	_opt_t = false;
	return 0;
}

// k 옵션 제거되면 0, 권한이 없으면 1 반환
int	Channel::minusOptK(Client* request_client) {
	if (checkAuthority(request_client) == false)
		return 1;
	_opt_k = "";
	return 0;
}

// l 옵션 제거되면 0, 권한이 없으면 1 반환
int	Channel::minusOptL(Client* request_client) {
	if (checkAuthority(request_client) == false)
		return 1;
	_opt_l = 0;
	return 0;
}

// o 옵션 제거되면 0, 권한이 없으면 1 반환, target이 없으면 2 반환
int	Channel::minusOptO(Client* request_client, Client* target_client) {
	if (checkAuthority(request_client) == false)
		return 1;
	std::map<Client *, bool>::iterator it = _user_list.find(target_client);
	if (it == _user_list.end()) {
		return 2;
	}
	it->second = false;
	return 0;
}

// join 성공하면 0, invite-only + invite 못 받았으면 1, key가 틀렸으면 2, 인원수 초과면 3 반환
int	Channel::join(Client* new_client, const std::string& key) {
	std::vector<std::string>::iterator it = std::find(_invite_list.begin(), _invite_list.end(), new_client->getNickname());
	if (_opt_i == true && it == _invite_list.end()) {
		new_client->setMessage(handleResponse(new_client->getNickname(), ERR_INVITEONLYCHAN));
		return ;
	}
	if (_opt_k != "" && _opt_k != key) {
		new_client->setMessage(handleResponse(new_client->getNickname(), ERR_BADCHANNELKEY));
		return ;
	}
	if (_opt_l != 0 && _user_list.size() >= _opt_l) {
		new_client->setMessage(handleResponse(new_client->getNickname(), ERR_CHANNELISFULL));
		return ;
	}
	if (_opt_i == true)
		_invite_list.erase(it);
	_user_list.insert(std::make_pair(new_client, false));
	new_client->setJoinedChannel(_name);
	// :<nick>!<user>@<host> JOIN <channel>
	std::string temp;
	temp = ":" + new_client->getNickname() + "!" + new_client->getUsername() \
				+ "@" + new_client->getHostname() + " JOIN " + _name;
	broadcast(temp);
	new_client->setMessage(handleResponse(new_client->getNickname(), RPL_TOPIC, _name, _topic));
	return 0;
}

// INVITE
void	Channel::invite(Client* request_client, Client* target_client) {
	if (checkChannelMember(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_NOTONCHANNEL));
		return ;
	}
	if (checkAuthority(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_CHANOPRIVSNEEDED));
		return ;
	}
	std::vector<std::string>::iterator it = std::find(_invite_list.begin(), _invite_list.end(), target_client->getNickname());
	if (it != _invite_list.end()) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_USERONCHANNEL));
		return ;
	}
	_invite_list.push_back(target_client->getNickname());
	std::string temp;
	// :<inviter_nick> INVITE <invitee_nick> <channel_name>
	temp = ":" + request_client->getNickname() + " INVITE " + target_client->getNickname() + " " + _name;
	target_client->setMessage(temp);
	broadcast(handleResponse(request_client->getNickname(), RPL_INVITING));
}

// kick 성공하면 0, 권한이 없으면 1, 없는 client면 2 반환
void	Channel::kick(Client* request_client, Client* target_client, const std::string& reason) {
	if (checkChannelMember(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_NOTONCHANNEL));
		return ;
	}
	if (checkAuthority(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_CHANOPRIVSNEEDED));
		return ;
	}
	std::map<Client *, bool>::iterator it = _user_list.find(target_client);
	if (it == _user_list.end()) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_USERNOTINCHANNEL));
		return ;
	}
	// :<nick>!<user>@<host> KICK <channel> <user> :<comment>
	std::string temp;
	temp = ":" + request_client->getNickname() + "!" + request_client->getUsername() \
				+ "@" + request_client->getHostname() + " KICK " + _name + " " \
				+ target_client->getUsername();
	if (reason != "")
		temp += " :" + reason;
	broadcast(temp);
	_user_list.erase(it);
	request_client->deleteJoinedChannel(_name);
}

// topic 설정 성공하면 0, topic-protection mode인데 권한이 없으면 1 반환
void	Channel::topic(Client* request_client, const std::string& topic) {
	if (checkChannelMember(request_client) == false) {
		request_client->setMessage(handleResponse(request_client->getNickname(), ERR_NOTONCHANNEL));
		return ;
	}
	if (topic == "") {
		if (_topic == "") {
			request_client->setMessage(handleResponse(request_client->getNickname(), RPL_NOTOPIC));
			return ;
		} else {
			request_client->setMessage(handleResponse(request_client->getNickname(), RPL_TOPIC, _name, _topic));
			return ;
		}
	} else {
		if (_opt_t == true && checkAuthority(request_client) == false) {
			request_client->setMessage(handleResponse(request_client->getNickname(), ERR_CHANOPRIVSNEEDED));
			return ;
		} else {
			// :<nick>!<user>@<host> TOPIC <channel> :<new topic>
			std::string temp;
			temp = ":" + request_client->getNickname() + "!" + request_client->getUsername() \
					+ "@" + request_client->getHostname() + " TOPIC " + _name + " :" \
					+ topic;
			broadcast(temp);
			_topic = topic;
			return ;
		}
	}
}

// quit을 했을 때 유저를 제거하고 메세지 보냄
void	Channel::quit(Client* request_client) {
	std::map<Client *, bool>::iterator it = _user_list.find(request_client);
	if (it != _user_list.end()) {
		_user_list.erase(it);
		std::string temp;
		temp = request_client->getNickname();
		temp += " has quit ";
		temp += _name;
		for (std::map<Client *, bool>::iterator i = _user_list.begin(); i != _user_list.end(); i++)
			i->first->setMessage(temp);
	}
}

// 에러로 quit을 했을 때 유저를 제거만 해줌
void	Channel::errorQuit(Client* request_client) {
	std::map<Client *, bool>::iterator it = _user_list.find(request_client);
	if (it != _user_list.end())
		_user_list.erase(it);
}

// 채널의 모든 클라이언트들에게 메세지를 보냄
void	Channel::broadcast(const std::string& message) {
	for (std::map<Client *, bool>::iterator i = _user_list.begin(); i != _user_list.end(); i++)
		i->first->setMessage(message);
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
