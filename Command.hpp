
#pragma once

#include "essential.hpp"
#include <string>

class Client;
class Channel;
class Server;

class Command {
//Command가 Server도 갖고 있어야함
public :
	Command();
	// Command(Client* client, const std::string& msg);
	void handleCmd(Client* client, const std::string& msg);//overloading
	void handleCmd(Server& server, Client* client, const std::string& msg);
	void handleCmd(Client* client, Channel* channel, const std::string& msg);
	bool parseCmd(const std::string& msg);
	void pass(Server& server, Client* client);
	void nick(Client* client);
	void user(Client* client);
	void join(Client* client, Channel* channel);

private :
	std::vector <std::string> _cmdlist;
	std::string _cmd;
	std::vector <std::string> _params;
};