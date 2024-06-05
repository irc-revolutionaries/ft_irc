
#pragma once

#include "essential.hpp"

class Client;

class Channel {
public :
	Channel(const std::string& name, Client* first_client);

	// command에서 호출 할 함수들
	int		join(Client* new_client, const std::string& key);
	int		invite(Client* request_client, Client* target_client);
	int		kick(Client* request_client, Client* target_client, const std::string& reason);
	int		topic(Client* request_client, const std::string& topic);
	void	part(Client* request_client);
	void	quit(Client* request_client);
	void	errorQuit(Client* request_client);

	// 채널 옵션 설정 함수들
	int	plusOptI(Client* request_client);
	int	plusOptT(Client* request_client);
	int	plusOptK(Client* request_client, const std::string& key);
	int	plusOptL(Client* request_client, const int limit);
	int	plusOptO(Client* request_client, Client* target_client);

	// 채널 옵션 제거 함수들
	int	minusOptI(Client* request_client);
	int	minusOptT(Client* request_client);
	int	minusOptK(Client* request_client);
	int	minusOptL(Client* request_client);
	int	minusOptO(Client* request_client, Client* target_client);

	// 채널에 있는 클라이언트 map을 확인하는 함수
	const std::map<Client *, bool>	&get_user_list(void) const;

	// getter 함수
	const std::string&	get_name(void) const;
	const std::string&	get_topic(void) const;

private :
	std::string					_name;
	std::map<Client *, bool>	_user_list;
	std::string					_topic;
	std::vector<std::string>	_invite_list;

	bool check_authority(Client* client);

	// 옵션 관련 flag들
	bool		_opt_i;
	bool		_opt_t;
	std::string	_opt_k;
	int			_opt_l;
};
