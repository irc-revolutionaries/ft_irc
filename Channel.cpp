#include "essential.hpp"
#include "Channel.hpp"

Channel::Channel(const std::string& name, Client* first_client) : _name(name), _opt_i(false), _opt_t(false), _opt_l(0) {
	_user_list.insert(std::pair<Client *, bool>(first_client, true));
}

// client가  채널에 없거나, 있는데 권한이 없으면 false
bool Channel::check_authority(Client* client) {
	std::map<Client *, bool>::iterator it = _user_list.find(client);
	if (it == _user_list.end()) { 
		return false;
	}
	return it->second;
}

// i 옵션 설정되면 0, 권한이 없으면 1 반환
int	Channel::plus_opt_i(Client* request_client) {
	if (check_authority(request_client) == false)
		return 1;
	_opt_i = true;
	return 0;
}

// t 옵션 설정되면 0, 권한이 없으면 1 반환
int	Channel::plus_opt_t(Client* request_client) {
	if (check_authority(request_client) == false)
		return 1;
	_opt_t = true;
	return 0;
}

// k 옵션 설정되면 0, 권한이 없으면 1 반환
int	Channel::plus_opt_k(Client* request_client, std::string key) {
	if (check_authority(request_client) == false)
		return 1;
	_opt_k = key;
	return 0;
}

// l 옵션 설정되면 0, 권한이 없으면 1 반환, 기존의 limit가 더 크거나 같으면 2 반환
int	Channel::plus_opt_l(Client* request_client, int limit) {
	if (check_authority(request_client) == false) {
		return 1;
	} else if (_opt_l >= limit) {
		return 2;
	}
	_opt_l = limit;
	return 0;
}

// o 옵션 설정되면 0, 권한이 없으면 1 반환, target이 없으면 2 반환
int	Channel::plus_opt_o(Client* request_client, Client* target_client) {
	if (check_authority(request_client) == false)
		return 1;
	std::map<Client *, bool>::iterator it = _user_list.find(target_client);
	if (it == _user_list.end()) {
		return 2;
	}
	it->second = true;
	return 0;
}

// i 옵션 제거되면 0, 권한이 없으면 1 반환
int	Channel::minus_opt_i(Client* request_client) {
	if (check_authority(request_client) == false)
		return 1;
	_opt_i = false;
	return 0;
}

// t 옵션 제거되면 0, 권한이 없으면 1 반환
int	Channel::minus_opt_t(Client* request_client) {
	if (check_authority(request_client) == false)
		return 1;
	_opt_t = false;
	return 0;
}

// k 옵션 제거되면 0, 권한이 없으면 1 반환
int	Channel::minus_opt_k(Client* request_client) {
	if (check_authority(request_client) == false)
		return 1;
	_opt_k = "";
	return 0;
}

// l 옵션 제거되면 0, 권한이 없으면 1 반환
int	Channel::minus_opt_l(Client* request_client) {
	if (check_authority(request_client) == false)
		return 1;
	_opt_l = 0;
	return 0;
}

// o 옵션 제거되면 0, 권한이 없으면 1 반환, target이 없으면 2 반환
int	Channel::minus_opt_o(Client* request_client, Client* target_client) {
	if (check_authority(request_client) == false)
		return 1;
	std::map<Client *, bool>::iterator it = _user_list.find(target_client);
	if (it == _user_list.end()) {
		return 2;
	}
	it->second = false;
	return 0;
}

// join 성공하면 0, invite-only면 1, key가 틀렸으면 2 반환
int	Channel::join(Client* new_client, std::string key) {
	if (_opt_i == true) {
		return 1;
	} else if (_opt_k != "" && _opt_k != key) {
		return 2;
	}
	_user_list.insert(std::make_pair(new_client, false));
	return 0;
}

// invite 성공하면 0, 권한이 없으면 1, 이미 등록된 client면 2 반환
int	Channel::invite(Client* request_client, Client* target_client) {
	if (check_authority(request_client) == false)
		return 1;
	std::map<Client *, bool>::iterator it = _user_list.find(target_client);
	if (it != _user_list.end())
		return 2;
	_user_list.insert(std::make_pair(target_client, false));
	return 0;
}

// kick 성공하면 0, 권한이 없으면 1, 없는 client면 2 반환
int	Channel::kick(Client* request_client, Client* target_client, std::string key) {
	if (check_authority(request_client) == false)
		return 1;
	std::map<Client *, bool>::iterator it = _user_list.find(target_client);
	if (it == _user_list.end())
		return 2;
	_user_list.erase(it);
	// broad cast message (kick 사유) 보내기!!!!!!!!
	return 0;
}

// topic 설정 성공하면 0, topic-protection mode인데 권한이 없으면 1 반환
int	Channel::topic(Client* request_client, std::string topic) {
	if (_opt_t == true && check_authority(request_client) == false)
		return 1;
	_topic = topic;
	return 0;
}

// part를 실행
void	Channel::part(Client* request_client) {
	std::map<Client *, bool>::iterator it = _user_list.find(request_client);
	if (it != _user_list.end())
		_user_list.erase(it);
}

// 클라이언트 map getter
const std::map<Client *, bool>	&Channel::get_user_list(void) const {
	return _user_list;
}
