
#pragma once

#include "essential.hpp"

class Client;

class Channel {
public :
	Channel(const std::string& name);

	// command에서 호출 할 함수들
	void	join(Client* new_client, const std::string& key);
	void	invite(Client* request_client, Client* target_client);
	void	kick(Client* request_client, Client* target_client, const std::string& reason);
	void	topic(Client* request_client, const std::string& topic);
	void	quit(Client* request_client, const std::string& message);
	void	errorQuit(Client* request_client);
	void	broadcast(const std::string& message);
	void	broadcastWithoutClient(const std::string& message, Client* client);
	void	answerMode(Client* request_client);

	// 채널 옵션 설정 함수들
	void	plusOptI(Client* request_client);
	void	plusOptT(Client* request_client);
	void	plusOptK(Client* request_client, const std::string& key);
	void	plusOptL(Client* request_client, std::size_t limit);
	void	plusOptO(Client* request_client, Client* target_client);

	// 채널 옵션 제거 함수들
	void	minusOptI(Client* request_client);
	void	minusOptT(Client* request_client);
	void	minusOptK(Client* request_client);
	void	minusOptL(Client* request_client);
	void	minusOptO(Client* request_client, Client* target_client);

	// 채널에 있는 클라이언트 map을 확인하는 함수
	const std::map<Client *, bool>	&getUserList(void) const;

	// getter 함수
	const std::string&	getName(void) const;
	const std::string&	getTopic(void) const;

	bool checkAuthority(Client* client);
private :
	std::string					_name;
	std::map<Client *, bool>	_user_list;
	std::string					_topic;
	std::vector<std::string>	_invite_list;

	bool checkChannelMember(Client* client);

	// 옵션 관련 flag들
	bool		_opt_i;
	bool		_opt_t;
	std::string	_opt_k;
	std::size_t	_opt_l;
};
