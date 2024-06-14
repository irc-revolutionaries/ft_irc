#include "Command.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "Bonus.hpp"
#include <sstream>

Command::Command() {
	_cmdlist.push_back("PASS");
	_cmdlist.push_back("NICK");
	_cmdlist.push_back("USER");
	_cmdlist.push_back("JOIN");
	_cmdlist.push_back("INVITE");
	_cmdlist.push_back("KICK");
	_cmdlist.push_back("TOPIC");
	_cmdlist.push_back("QUIT");
	_cmdlist.push_back("PRIVMSG");
	_cmdlist.push_back("MODE");
	_cmdlist.push_back("PING");
}

void Command::handleCmd(Server& server, Client* client, const std::string& msg) {
	if (parseCmd(client, msg) == false)
		return ;
	if (_cmd == "PING")	{
		ping(client);
		return ;
	}
	if (_cmd == "PASS")
		pass(server, client);
	else if (client->getPass()) {
		if (_cmd == "NICK")
			nick(server, client);
		else if (_cmd == "USER")
			user(client);
		else if (client->getAllReady()) {
			if (_cmd == "JOIN")
				join(server, client);
			else if (_cmd == "INVITE")
				invite(server, client);
			else if (_cmd == "KICK")
				kick(server, client);
			else if (_cmd == "TOPIC")
				topic(server, client);
			else if (_cmd == "QUIT")
				quit(server, client);
			else if (_cmd == "PRIVMSG")
				privmsg(server, client);
			else if (_cmd == "MODE")
				mode(server, client);
			else {
				client->setMessage(handleResponse(client->getNickname(), ERR_UNKNOWNCOMMAND, _cmd));
			}
		}
		else {
			client->setMessage(handleResponse("*", ERR_NOTREGISTERED));
			return ;
		}
	}
	else {
		//pass를 입력안해서nick이 없는데 어떻게 response?
		// 정보가 없을때 nickname 을 *로 표시
		client->setMessage(handleResponse("*", ERR_NOTREGISTERED));
		return ;
	}
}

bool Command::parseCmd(Client* client, const std::string& msg) {
	_params.clear();
	_cmd.clear();
	std::istringstream ss(msg);
	std::string buf;
	std::string tmp1;
	std::string col;
	(void)client;

	std::getline(ss, tmp1, ':');
	std::getline(ss, col);
	std::istringstream col_ss(tmp1);
	std::getline(col_ss, _cmd, ' ');
	if (std::find(_cmdlist.begin(), _cmdlist.end(), _cmd) == _cmdlist.end()){
		//421
		if (client->getNick())
			client->setMessage(handleResponse(client->getNickname(), ERR_UNKNOWNCOMMAND ,_cmd));
		else 
			client->setMessage(handleResponse("*", ERR_UNKNOWNCOMMAND, _cmd));
		return false;
	}
	while (std::getline(col_ss, buf, ' ')) {
		_params.push_back(buf);
	}
	if (col != "")
		_params.push_back(col);
	std::cout << "cmd : " << _cmd <<'\n';
	for (int i = 0; i < (int)_params.size(); ++i) {
		std::cout << "params " << i << " : "<< _params[i] << std::endl;
	}
	return true;
}

void Command::pass(Server& server, Client* client) {
	//PASS <password>
	if (_params.empty()) {
		client->setMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "PASS"));
		client->setDisconnect(true);
		return ;
	}
	if (client->getPass()) {
		client->setMessage(handleResponse(client->getNickname(), ERR_ALREADYREGISTRED));
		return ;
	}
	if (_params[0] == server.getPassword()) {
		client->setPass(true);
	}
	else {
		client->setMessage(handleResponse("", ERR_PASSWDMISMATCH));
		client->setDisconnect(true);
	}
}

void Command::nick(Server& server, Client* client) {
	//NICK <nickname>
	//want 코크풍선..
	//NICK 변경가능
	if (client->getNick()) {
		client->setMessage(handleResponse(client->getNickname(), ERR_UNKNOWNCOMMAND, "NICK"));
		return ;
	}
	if (_params.empty()) {
		client->setMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "NICK"));
		return ;
	}
	std::string nickname = _params[0];
	if (nickname[0] == '$' || nickname[0] == ':' || nickname[0] == '#' || nickname[0] == '&'
		|| std::isdigit(nickname[0])) {
		client->setMessage(handleResponse("*", ERR_ERRONEUSNICKNAME, nickname));
		return ;
	}
	for (size_t i = 0; i < nickname.size(); ++i) {
		if (nickname[i] == ' ' || nickname[i] == ',' || nickname[i] == '.' || nickname[i] == '*' ||
			nickname[i] == '?' || nickname[i] == '!' || nickname[i] == '@') {
			client->setMessage(handleResponse("*", ERR_ERRONEUSNICKNAME, nickname));
			return ;
		}
	}
	if (nickname.length() > 9) {
		client->setMessage(handleResponse("*", ERR_ERRONEUSNICKNAME, nickname));
		return ;
	}
	if (server.findClient(nickname) || nickname == "bot") {
		client->setMessage(handleResponse("*", ERR_NICKNAMEINUSE, nickname));
		return ;
	}
	std::cout << client << '\n';
	client->setNickname(nickname);
	client->setNick(true);
	client->setMessage(messageFormat(NICK, client, nickname));
	if (client->getNick() && client->getUser() && !client->getAllReady())
		allready(client);
}

void Command::user(Client* client) {
	//USER <username> <hostname> <servername> :<realname>
	if (client->getUser()) {
		client->setMessage(handleResponse(client->getNickname(), ERR_UNKNOWNCOMMAND, "USER"));
		return ;
	}
	if (_params.size() < 4) {
		client->setMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "USER"));
		return ;
	}
	if (client->getUser()) {
		client->setMessage(handleResponse(client->getNickname(), ERR_ALREADYREGISTRED));
		return ;
	}
	client->setUser(true);
	client->setUsername(_params[0]);
	client->setHostname(_params[1]);
	client->setServername(_params[2]);
	client->setRealname(_params[3]);
	if (client->getNick() && client->getUser() && !client->getAllReady())
		allready(client);
}

void	Command::allready(Client* client) {
	client->setMessage(messageFormat(RPL_WELCOME, client));
	client->setMessage(messageFormat(RPL_YOURHOST, client));
	client->setMessage(messageFormat(RPL_CREATED, client, "Mon Jan 1 00:00:00 2020"));
	client->setMessage(messageFormat(RPL_MYINFO, client, "tmp1.0 x itklo"));
	client->setAllReady(true);
}

void Command::join(Server& server, Client* client) {
	//JOIN <channel>{,<channel>} [<key>{,<key>}]
	//JOIN c1,c2,c3,c4 k1,k2 c4 = "hello,hi"
	//채널이름에 ',' , 7번, 공백 사용금지
	//JOIN 채널이름이 없으면 만들어서 server의 chanel
	if (_params.empty()) {
		client->setMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "JOIN"));
		return ;
	}
	std::map<std::string, std::string> key_map;
	std::map<std::string, Channel *> channel_list = server.getChannelList();
	std::istringstream channel_ss(_params[0]);
	std::istringstream key_ss(_params.size() > 1 ? _params[1] : "");
	std::string channel_name;
	std::string key;

	while (std::getline(channel_ss, channel_name, ',')) {
		if (channel_name[0] != '#'){
			client->setMessage(handleResponse(channel_name, ERR_BADCHANMASK));
			std::getline(key_ss, key, ',');//< - 이거 괜찮음?
			continue ;
		}
		if (channel_name.find(6) != std::string::npos
			|| channel_name.find(':') != std::string::npos)
			client->setMessage(handleResponse(client->getNickname(), ERR_NOSUCHCHANNEL, channel_name));
		if (!std::getline(key_ss, key, ','))
			key = "";
		key_map[channel_name] = key;
	}

	std::map<std::string, std::string>::iterator it = key_map.begin();
	for (; it != key_map.end(); ++it){
		std::string channel_name = it->first;
		std::string channel_key = it->second;
		if (channel_list.find(channel_name) != channel_list.end()) {
			std::vector<std::string> joined_ch = client->getJoinedChannel();
			if (std::find(joined_ch.begin(), joined_ch.end(), channel_name) != joined_ch.end()) {
				client->setMessage(handleResponse(client->getNickname(), ERR_USERONCHANNEL, channel_name));
				continue ;
			}
			channel_list[channel_name]->join(client, channel_key);
		}
		else{
			server.createChannel(channel_name);
			std::map<std::string, Channel *> channel_list1 = server.getChannelList();
			channel_list1[channel_name]->join(client, channel_key);
		}
	}
}

void	Command::invite(Server& server, Client* client){
	//INVITE <nickname> <channel>
	if (_params.size() < 2) {
		client->setMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "INVITE"));
        return;
    }
	std::map<std::string, Channel *> channel_list = server.getChannelList();
	Client* invited_client = server.findClient(_params[0]);
	if (!invited_client) {
		client->setMessage(handleResponse(client->getNickname(), ERR_NOSUCHNICK, _params[0]));
		return ;
	}
	if (channel_list.find(_params[1]) == channel_list.end()){
		client->setMessage(handleResponse(client->getNickname(), ERR_NOSUCHCHANNEL, _params[1]));
		return ;
	}
	channel_list[_params[1]]->invite(client, invited_client);
}

void	Command::kick(Server& server, Client* client){
	// KICK <channel> <user> [<comment>]
	std::map<std::string, Channel *> channel_list = server.getChannelList();

	if (_params.size() < 2) {
		client->setMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "KICK"));
		return ;
	}
	std::string channel_name = _params[0];
	std::string target_name = _params[1];
	if (!server.findClient(target_name)) {
		client->setMessage(handleResponse(client->getNickname(), ERR_NOSUCHNICK, target_name));
		return ;
	}
	if (channel_list.find(channel_name) == channel_list.end()) {
		client->setMessage(handleResponse(client->getNickname(), ERR_NOSUCHNICK, channel_name));
		return ;
	}
	if (_params.size() >= 3) {
		std::string comment = _params[2];
		channel_list[channel_name]->kick(client, server.findClient(target_name), comment);
	}
	else
		channel_list[channel_name]->kick(client, server.findClient(target_name), "");
	if (channel_list[channel_name]->getUserList().size() == 0)
		server.deleteChannelList(channel_name);
}

void	Command::topic(Server& server, Client* client) {
	//TOPIC <channel> [<topic>]
	//<topic>이 주어지지 않으면 topic 반환
	std::map<std::string, Channel *> channel_list = server.getChannelList();

	if (_params.size() < 1) {
		client->setMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "TOPIC"));
		return ;
	}
	std::string channel_name = _params[0];
	if (channel_list.find(channel_name) == channel_list.end()) {
		client->setMessage(handleResponse(client->getNickname(), ERR_NOSUCHCHANNEL, channel_name));
		return ;
	}
	if (_params.size() >= 2) {
		std::string topic = _params[1];
		channel_list[channel_name]->topic(client, topic);
	}
	else
		channel_list[channel_name]->topic(client, "");
}

void	Command::quit(Server& server, Client* client) {
	//QUIT <quit message>
	std::map<std::string, Channel *> channel_list = server.getChannelList();
	std::vector<std::string> joined_channel = client->getJoinedChannel();
	if (_params.empty()) {
		client->setMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "QUIT"));
		return ;
	}
	for (unsigned int i = 0; i < joined_channel.size(); ++i) {
		channel_list[joined_channel[i]]->quit(client, _params[0]);
		if (channel_list[joined_channel[i]]->getUserList().size() == 0) {
			server.deleteChannelList(joined_channel[i]);
		}
	}
	client->clearJoinedChannel();
	client->setDisconnect(true);
}

void	Command::privmsg(Server& server, Client* client) {
	//PRIVMSG <receiver>{,<receiver>} <text to be sent>
	std::map<std::string, Channel *> channel_list = server.getChannelList();
	std::vector<std::string> joined_channel = client->getJoinedChannel();

	if (_params.size() < 2) {
		client->setMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "PRIVMSG"));
		return ;
	}
	std::string receiver;
	std::string msg = _params[1];
	std::istringstream ss(_params[0]);
	while (std::getline(ss, receiver, ',')) {
		std::vector<std::string>::iterator it = std::find(joined_channel.begin(), joined_channel.end(), receiver);
		if (receiver[0] == '#') {
			if (channel_list.find(receiver) == channel_list.end())
				client->setMessage(handleResponse(client->getNickname(), ERR_NOSUCHCHANNEL, receiver));
			else if (it == joined_channel.end())
				client->setMessage(handleResponse(client->getNickname(), ERR_CANNOTSENDTOCHAN, receiver));
			else
				channel_list[receiver]->broadcastWithoutClient(messageFormat(PRIVMSG, client, receiver, msg), client);//여기에다가 포맷 맞춰서 보내기
		} else {
			if (receiver == "bot") {
				excuteBot(client);
				return ;
			}
			Client* receive_client = server.findClient(receiver);
			if (!receive_client)
				client->setMessage(handleResponse(client->getNickname(), ERR_NOSUCHNICK, receiver));
			else
				receive_client->setMessage(messageFormat(PRIVMSG, client, receive_client->getNickname(), msg));
		}
	}
}

void	Command::mode(Server& server, Client* client) {
	//MODE <channel> {[+|-]|i|t|k|o|l} [<limit>] [<user>]
	//1. + - 인식 없으면 에러
	//2 + or - 둘중 하나만 받고 이후에 있는 옵션들 다적용
	//파라미터 없는 모드 : i, t
	//파라미터 있는 모드 : k, o, l
	std::map<std::string, Channel *> channel_list = server.getChannelList();
	if (_params.size() < 1) {
		client->setMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "MODE"));
		return ;
	}
	if (_params.size() == 1) {
		if (channel_list.find(_params[0]) != channel_list.end()) {
			channel_list[_params[0]]->answerMode(client);
			return ;
		} else {
			client->setMessage(handleResponse(client->getNickname(), ERR_NOSUCHCHANNEL, _params[0]));
			return ;
		}
	}
	std::string	channel_name = _params[0];
	if (channel_list.find(channel_name) == channel_list.end()) {
		client->setMessage(handleResponse(client->getNickname(), ERR_NOSUCHCHANNEL, channel_name));
		return ;
	}
	std::string opt = _params[1];
	size_t pos_plus = opt.find('+');
	size_t pos_minus = opt.find('-');
	if (pos_plus == std::string::npos && pos_minus == std::string::npos) {
		// UNKNOWNMODE 472
		client->setMessage(handleResponse(client->getNickname(), ERR_UNKNOWNMODE, opt));
		return ;
	}
	std::string reply;
	std::string params_reply;
	bool opt_i = false;
	bool opt_t = false;
	bool opt_k = false;
	bool opt_o = false;
	bool opt_l = false;
	bool plus = false;
	bool minus = false;
	if (opt[0] == '+')
		plus = true;
	else if (opt[0] == '-')
		minus = true;
	std::size_t	params_order = 2;
	size_t j = 0;

	std::map<Client *, bool> user_list = channel_list[channel_name]->getUserList();
	if (channel_list[channel_name]->checkChannelMember(client) == false) {
		client->setMessage(handleResponse(client->getNickname(), ERR_NOTONCHANNEL, channel_name));
		return ;
	}
	if (channel_list[channel_name]->checkAuthority(client) == false) {
		client->setMessage(handleResponse(client->getNickname(), ERR_CHANOPRIVSNEEDED, channel_name));
		return ;
	}
	for (size_t i = 0; i < opt.size();) {
		if (opt[i] == '+') {
			for (; j < opt.size(); j++) {
				if (opt[j] == 'i') {
					if (opt_i)
						continue ;
					channel_list[channel_name]->plusOptI();
					opt_i =	true;
					if (plus) {
						reply += "+";
						plus = false;
					}
					reply += "i";
				} else if (opt[j] == 't') {
					if (opt_t)
						continue ;
					channel_list[channel_name]->plusOptT();
					opt_t = true;
					if (plus) {
						reply += "+";
						plus = false;
					}
					reply += "t";
				} else if (opt[j] == 'k') {
					if (opt_k)
						continue ;
					if (params_order == _params.size()) {
						client->setMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "MODE"));
						continue ;
					}
					channel_list[channel_name]->plusOptK(_params[params_order]);
					opt_k = true;
					if (plus) {
						reply += "+";
						plus = false;
					}
					reply += "k";
					params_reply += " " + _params[params_order];
					++params_order;
				} else if (opt[j] == 'o') {
					if (opt_o)
						continue ;
					if (params_order == _params.size()) {
						client->setMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "MODE"));
						continue ;
					}
					Client* target_client = server.findClient(_params[params_order]);
					if (!target_client) {
						client->setMessage(handleResponse(client->getNickname(), ERR_USERNOTINCHANNEL, _params[params_order], channel_name));
						++params_order;
						continue ;
					}
					channel_list[channel_name]->plusOptO(client, target_client);
					opt_o = true;
					if (plus) {
						reply += "+";
						plus = false;
					}
					reply += "o";
					params_reply += " " + _params[params_order];
					++params_order;
				} else if (opt[j] == 'l') {
					if (opt_l)//params_order++?
						continue ;
					if (params_order == _params.size()) {
						client->setMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "MODE"));
						continue ;
					}
					long nb = std::atol(_params[params_order].c_str());
					if (nb <= 0) {
						client->setMessage(handleResponse(client->getNickname(), ERR_UNKNOWNMODE, _params[params_order]));
						params_order++;
						continue ;
					}
					channel_list[channel_name]->plusOptL(nb);
					opt_l = true;
					if (plus) {
						reply += "+";
						plus = false;
					}
					reply += "l";
					std::stringstream tostring;
					tostring << nb;
					params_reply += " " + tostring.str();
					++params_order;
				} else if (opt[j] == '-') {
					minus = true;
					break ;
				} else if (opt[j] == '+') {
					continue ;
				} else {
					client->setMessage(handleResponse(client->getNickname(), ERR_UNKNOWNMODE, std::string(1, opt[j])));
					continue ;
				}
			}
		} else if (opt[i] == '-') {
			for (; j < opt.size(); j++) {
				if (opt[j] == 'i') {
					if (opt_i)
						continue ;
					channel_list[channel_name]->minusOptI();
					opt_i =	true;
					if (minus) {
						reply += "-";
						minus = false;
					}
					reply += "i";
				} else if (opt[j] == 't') {
					if (opt_t)
						continue ;
					channel_list[channel_name]->minusOptT();
					opt_t = true;
					if (minus) {
						reply += "-";
						minus = false;
					}
					reply += "t";
				} else if (opt[j] == 'k') {
					if (opt_k)
						continue ;
					channel_list[channel_name]->minusOptK();
					opt_k = true;
					if (minus) {
						reply += "-";
						minus = false;
					}
					reply += "k";
				} else if (opt[j] == 'o') {
					if (opt_o)
						continue ;
					if (params_order == _params.size()) {
						client->setMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "MODE"));
						continue ;
					}
					Client* target_client = server.findClient(_params[params_order]);
					if (!target_client) {
						client->setMessage(handleResponse(client->getNickname(), ERR_USERNOTINCHANNEL, _params[params_order], channel_name));
						++params_order;
						continue ;
					}
					channel_list[channel_name]->minusOptO(client, target_client);
					opt_o = true;
					if (minus) {
						reply += "-";
						minus = false;
					}
					reply += "o";
					params_reply += " " + _params[params_order];
					++params_order;
				} else if (opt[j] == 'l') {
					if (opt_l)
						continue ;
					channel_list[channel_name]->minusOptL();
					opt_l = true;
					if (minus) {
						reply += "-";
						minus = false;
					}
					reply += "l";
				} else if (opt[j] == '+') {
					plus = true;
					break ;
				} else if (opt[j] == '-') {
					continue ;
				} else {
					client->setMessage(handleResponse(client->getNickname(), ERR_UNKNOWNMODE, std::string(1, opt[j])));
					continue ;
				}
			}
		}
		if (opt[i] != '+' && opt[i] != '-')
			i++;
		else
			i = j;
		std::cout << opt << ' ' << i << ' ' << j << '\n';
	}
	reply += params_reply;
	if (!(reply == "+" || reply == "-"))
		channel_list[channel_name]->broadcast(messageFormat(MODE, client, channel_name, reply));
}

void	Command::ping(Client* client) {
	if (_params.size() != 1) 
		return ;
	client->setMessage(messageFormat(PONG, client));
}
