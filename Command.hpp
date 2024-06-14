
#pragma once

#include "essential.hpp"
#include <string>

class Client;
class Channel;
class Server;

class Command {

public :
	Command();
	void	handleCmd(Server& server, Client* client, const std::string& msg);
	bool	parseCmd(Client* client, const std::string& msg);
	void	pass(Server& server, Client* client);
	void	nick(Server& server, Client* client);
	void	user(Client* client);
	void	join(Server& server, Client* client);
	void	invite(Server& server, Client* client);
	void	kick(Server& server, Client* client);
	void	topic(Server& server, Client* client);
	void	quit(Server& server, Client* client);
	void	privmsg(Server& server, Client* client);
	void	mode(Server& server, Client* client);
	void	part(Server& server, Client* client);
	void	ping(Client* client);
	void	allready(Client* client);
	bool	validNick(Client* client, const std::string& nickname);

private :
	std::vector <std::string> _cmdlist;
	std::string _cmd;
	std::vector <std::string> _params;
};