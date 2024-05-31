
#include "Command.hpp"
#include "Client.hpp"
#include <sstream>

Command::Command(Client& client, const std::string& msg) : _msg(msg) {
	parseCmd();
	workCmd(client);
}

void Command::parseCmd() {
	std::istringstream ss(_msg);

	std::getline(ss, _cmd, ' ');//command
	std::getline(ss, _params);
}
void Command::workCmd(Client& client) {
	if (_cmd == "PASS")
		pass(client, _params);
	else if (_cmd == "NICK")
		nick(client, _params);
	else if (_cmd == "USER")
		user(client, _params);
	else if (_cmd == "JOIN")
		join(client, _params);
}

void Command::pass(Client& client, const std::string& params) {
	//PASS <password>
	if (params == "password")
		client.setPass(true);
}

void Command::nick(Client& client, const std::string& params) {
	//NICK <nickname>
	if (params[0] == '$' || params[0] == ':' || params[0] == '#' || params[0] == '&') {
		std::cerr << "nick wrong params\n";
		exit(1);
	}
	for (size_t i = 0; params.length(); ++i) {
		if (params[i] == ' ' || params[i] == ',' || params[i] == '.' || params[i] == '*' ||
			params[i] == '?' || params[i] == '!' || params[i] == '@') {
			std::cerr << "nick wrong params\n";
			exit(1);
		}
	}
	//중복 닉네임 고려 어떻게?
	client.setNickname(params);
}

void Command::user(Client& client, const std::string& params) {
	std::istringstream ss(params);
	//USER <username> <hostname> <servername> :<realname>
}

void Command::join(Client& client, const std::string& params) {
	//JOIN <channel>{,<channel>} [<key>{,<key>}]
	//한번에 두개 채널 접근한다는거같음
}

