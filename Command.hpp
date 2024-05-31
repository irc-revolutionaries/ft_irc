
#pragma once

#include "essential.hpp"
#include <string>

class Client;

class Command {
//Command가 Server도 갖고 있어야함
	public :
		Command(Client& client, const std::string& msg);
		void parseCmd();
		void workCmd(Client& client);
		void pass(Client& client, const std::string& params);
		void nick(Client& client, const std::string& params);
		void user(Client& client, const std::string& params);
		void join(Client& client, const std::string& params);

	private :
		std::vector <std::string> _cmdlist;
		std::string _msg;
		std::string _cmd;
		std::string _params;
};