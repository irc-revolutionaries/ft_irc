
#include "Command.hpp"
#include "Client.hpp"
#include <sstream>

Command::Command() {
	_cmdlist.push_back("/join");
	_cmdlist.push_back("/pass");
	_cmdlist.push_back("/user");
	_cmdlist.push_back("/nick");
	_cmdlist.push_back("/quit");// 서버 나가면서 메세지 뱉기 
	_cmdlist.push_back("/part");// leave channel
//==============CHANNEL===========
	_cmdlist.push_back("/topic");
	_cmdlist.push_back("/invite");
	_cmdlist.push_back("/kick");
	_cmdlist.push_back("/mode");
//===============SERVER===========
	// _cmdlist.push_back("LIST");
	// _cmdlist.push_back("NAMES");
	_cmdlist.push_back("/privmsg");
}

void Command::handleCmd(Client* client, const std::string& msg) {
//msg  512자 넘으면 거절
//이 함수를 통해서 command 호출
	if (parseCmd(msg) == false)
		return ;
	if (_cmd == "/nick")
		nick(client);
	else if (_cmd == "/user")
		nick(client);
	//work
}
void Command::handleCmd(Client* client, Channel* channel, const std::string& msg) {
//msg  512자 넘으면 거절
//이 함수를 통해서 command 호출
	if (parseCmd(msg) == false)
		return ;
	if (_cmd == "/join")
		join(client, channel);
	//work
}

void Command::handleCmd(Server& server, Client* client, const std::string& msg) {
//이 함수를 통해서 command 호출
	if (parseCmd(msg) == false)
		return ;
	if (_cmd == "/pass")
		pass(server, client);
	//work
}

bool Command::parseCmd(const std::string& msg) {
	std::istringstream ss(msg);
	std::string buf;
	std::getline(ss, _cmd, ' ');//command
	if (std::find(_cmdlist.begin(), _cmdlist.end(), _cmd) == _cmdlist.end()){
		std::cerr << "wrong cmd\n";
		return false;
	}
	while (std::getline(ss, buf)){
		_params.push_back(buf);
	}
	return true;
}

void Command::pass(Server& server, Client* client) {
	//PASS <password>
	if (_params[0] == server.getPassword())
		client->setPass(true);
	else
		throw std::runtime_error("WRONG PASSWORD");
}

void Command::nick(Client* client) {
	std::string nickname;

	nickname = _params[0];
	//NICK <nickname>
	//want 코크풍선..
	if (nickname[0] == '$' || nickname[0] == ':' || nickname[0] == '#' || nickname[0] == '&') {
		std::cerr << "nick wrong nickname\n";
		exit(1);
	}
	for (size_t i = 0; nickname.length(); ++i) {
		if (nickname[i] == ' ' || nickname[i] == ',' || nickname[i] == '.' || nickname[i] == '*' ||
			nickname[i] == '?' || nickname[i] == '!' || nickname[i] == '@') {
			std::cerr << "nick wrong params\n";
			exit(1);
		}
	}
	//중복 닉네임 고려 어떻게?
	if (nickname.length() > 9)
		exit(1);
	client->setNickname(nickname);
}

void Command::user(Client* client) {
	if (_params.size() != 4)
		return ;
	client->setUsername(_params[0]);
	client->setHostname(_params[1]);
	client->setServername(_params[2]);
	client->setRealname(_params[3]);
	//USER <username> <hostname> <servername> :<realname>
}

void Command::join(Client* client, Channel* channel) {
	//JOIN <channel>{,<channel>} [<key>{,<key>}]
	//JOIN c1 c2 c3 c4 k1
	std::map<std::string, std::string> key_map;
	std::istringstream channel_ss(_params[0]);
	std::istringstream key_ss(_params[1]);
	std::string buf1;
	std::string buf2;
	while (std::getline(channel_ss, buf1, ',')) {
		if (!std::getline(key_ss, buf2, ','))
			buf2 = "";
		key_map[buf1] = buf2;
	}
	// ?
	//키 없어도 넣어도됨
}
