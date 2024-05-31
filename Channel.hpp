
#pragma once

#include "essential.hpp"

class Client;

class Channel {
public :
	Channel(const std::string& name, Client* first_client);

	// command에서 호출 할 함수들
	int		join(Client* new_client, std::string key);
	int		invite(Client* request_client, Client* target_client);
	int		kick(Client* request_client, Client* target_client, std::string );
	int		topic(Client* request_client, std::string topic);
	void	part(Client* request_client);

	// 채널 옵션 설정 함수들
	int	plus_opt_i(Client* request_client);
	int	plus_opt_t(Client* request_client);
	int	plus_opt_k(Client* request_client, std::string key);
	int	plus_opt_l(Client* request_client, int limit);
	int	plus_opt_o(Client* request_client, Client* target_client);

	// 채널 옵션 제거 함수들
	int	minus_opt_i(Client* request_client);
	int	minus_opt_t(Client* request_client);
	int	minus_opt_k(Client* request_client);
	int	minus_opt_l(Client* request_client);
	int	minus_opt_o(Client* request_client, Client* target_client);

	// 채널에 있는 클라이언트 map을 확인하는 함수
	const std::map<Client *, bool>	&get_user_list(void) const;

private :
	std::string					_name;
	std::map<Client *, bool>	_user_list;
	std::string					_topic;

	bool check_authority(Client* client);

	// 옵션 관련 flag들
	bool		_opt_i;
	bool		_opt_t;
	std::string	_opt_k;
	int			_opt_l;
};
