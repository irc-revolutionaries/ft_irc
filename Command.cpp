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
	_cmdlist.push_back("PART");
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
			else if (_cmd == "PART")
				part(server, client);
			else {
				client->addMessage(handleResponse(client->getNickname(), ERR_UNKNOWNCOMMAND, _cmd));
			}
		}
		else {
			client->addMessage(handleResponse(client->getNickname(), ERR_NOTREGISTERED));
			return ;
		}
	}
}

bool Command::parseCmd(Client* client, const std::string& msg) {
	_params.clear();
	_cmd.clear();
	std::istringstream ss(msg);
	std::string buf;
	std::string col;
	
	std::getline(ss, _cmd, ' ');
	if (std::find(_cmdlist.begin(), _cmdlist.end(), _cmd) == _cmdlist.end()){
		client->addMessage(handleResponse(client->getNickname(), ERR_UNKNOWNCOMMAND ,_cmd));
		return false;
	}
	while (std::getline(ss, buf, ' ')) {
		if (buf[0] == ':') {
			col = buf;
			break ;
		}
		_params.push_back(buf);
	}
	while (std::getline(ss, buf, ' ')) {
		col += (' ' + buf);
	}
	if (col != "")
		_params.push_back(col);
	return true;
}

void Command::pass(Server& server, Client* client) {
	//PASS <password>
	if (_params.empty()) {
		client->addMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "PASS"));
		return ;
	}
	if (client->getPass()) {
		client->addMessage(handleResponse(client->getNickname(), ERR_ALREADYREGISTRED));
		return ;
	}
	if (_params[0] == server.getPassword()) {
		client->setPass(true);
	}
	else
		client->addMessage(handleResponse("", ERR_PASSWDMISMATCH));
}

bool Command::validNick(Client* client, const std::string& nickname) {
	if (nickname[0] == '$' || nickname[0] == ':' || nickname[0] == '#' || nickname[0] == '&'
		|| std::isdigit(nickname[0])) {
		client->addMessage(handleResponse(client->getNickname(), ERR_ERRONEUSNICKNAME, nickname));
		return false;
	}
	for (std::size_t i = 0; i < nickname.size(); ++i) {
		if (nickname[i] == ' ' || nickname[i] == ',' || nickname[i] == '.' || nickname[i] == '*' ||
			nickname[i] == '?' || nickname[i] == '!' || nickname[i] == '@') {
			client->addMessage(handleResponse(client->getNickname(), ERR_ERRONEUSNICKNAME, nickname));
			return false;
		}
	}
	if (nickname.length() > 9 || nickname.length() == 0) {
		client->addMessage(handleResponse(client->getNickname(), ERR_ERRONEUSNICKNAME, nickname));
		return false;
	}
	return true;
}

void Command::nick(Server& server, Client* client) {
	//NICK <nickname>
	//NICK 변경가능
	if (_params.empty()) {
		client->addMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "NICK"));
		return ;
	}
	std::string nickname = _params[0];
	if (!validNick(client, nickname))
		return ;
	if (server.findClient(nickname) || nickname == "bot") {
		client->addMessage(handleResponse(client->getNickname(), ERR_NICKNAMEINUSE, nickname));
		return ;
	}
	if (client->getNick()) {
		std::map<std::string, Channel *> channel_list = server.getChannelList();
		std::map<std::string, Channel *>::iterator it = channel_list.begin();
		for (; it != channel_list.end(); it++)
			it->second->changeInviteNick(client->getNickname(), nickname);
		client->addMessage(messageFormat(NICK, client, nickname));
	}
	client->setNickname(nickname);
	client->setNick(true);
	if (client->getNick() && client->getUser() && !client->getAllReady())
		allready(client);
}

void Command::user(Client* client) {
	//USER <username> <hostname> <servername> :<realname>
	if (client->getUser()) {
		client->addMessage(handleResponse(client->getNickname(), ERR_UNKNOWNCOMMAND, "USER"));
		return ;
	}
	if (_params.size() < 4) {
		client->addMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "USER"));
		return ;
	}
	if (client->getUser()) {
		client->addMessage(handleResponse(client->getNickname(), ERR_ALREADYREGISTRED));
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
	client->addMessage(messageFormat(RPL_WELCOME, client));
	client->addMessage(messageFormat(RPL_YOURHOST, client));
	client->addMessage(messageFormat(RPL_CREATED, client, "SUN June 16 42:42:42 2024"));
	client->addMessage(messageFormat(RPL_MYINFO, client, "version 1.0 x itklo"));
	client->setAllReady(true);
}

void Command::join(Server& server, Client* client) {
	//JOIN <channel>{,<channel>} [<key>{,<key>}]
	if (_params.empty()) {
		client->addMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "JOIN"));
		return ;
	}
	std::map<std::string, Channel *> channel_list = server.getChannelList();
	std::istringstream channel_ss(_params[0]);
	std::istringstream key_ss(_params.size() > 1 ? _params[1] : "");
	std::string channel_name;
	std::string key;

	while (std::getline(channel_ss, channel_name, ',')) {
		if (!std::getline(key_ss, key, ','))
			key = "";
		if (channel_name[0] != '#'){
			client->addMessage(handleResponse(channel_name, ERR_BADCHANMASK));
			continue ;
		}
		if (channel_name.find(7) != std::string::npos) {
			client->addMessage(handleResponse(client->getNickname(), ERR_BADCHANNAME, channel_name));
			continue;
		}
		if (channel_list.find(channel_name) != channel_list.end()) {
			std::vector<std::string> joined_ch = client->getJoinedChannel();
			if (std::find(joined_ch.begin(), joined_ch.end(), channel_name) != joined_ch.end()) {
				client->addMessage(handleResponse(client->getNickname(), ERR_USERONCHANNEL, channel_name));
				continue ;
			}
			channel_list[channel_name]->join(client, key);
		} else {
			server.createChannel(channel_name);
			std::map<std::string, Channel *> channel_list1 = server.getChannelList();
			channel_list1[channel_name]->join(client, key);
		}
	}
}

void	Command::invite(Server& server, Client* client){
	//INVITE <nickname> <channel>
	if (_params.size() < 2) {
		client->addMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "INVITE"));
        return;
    }
	std::map<std::string, Channel *> channel_list = server.getChannelList();
	Client* invited_client = server.findClient(_params[0]);
	if (!invited_client) {
		client->addMessage(handleResponse(client->getNickname(), ERR_NOSUCHNICK, _params[0]));
		return ;
	}
	if (channel_list.find(_params[1]) == channel_list.end()){
		client->addMessage(handleResponse(client->getNickname(), ERR_NOSUCHCHANNEL, _params[1]));
		return ;
	}
	channel_list[_params[1]]->invite(client, invited_client);
}

void	Command::kick(Server& server, Client* client){
	// KICK <channel> <user> [<comment>]
	std::map<std::string, Channel *> channel_list = server.getChannelList();

	if (_params.size() < 2) {
		client->addMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "KICK"));
		return ;
	}
	std::string channel_name = _params[0];
	std::string target_name = _params[1];
	if (!server.findClient(target_name)) {
		client->addMessage(handleResponse(client->getNickname(), ERR_NOSUCHNICK, target_name));
		return ;
	}
	if (channel_list.find(channel_name) == channel_list.end()) {
		client->addMessage(handleResponse(client->getNickname(), ERR_NOSUCHNICK, channel_name));
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

	if (_params.empty()) {
		client->addMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "TOPIC"));
		return ;
	}
	std::string channel_name = _params[0];
	if (channel_list.find(channel_name) == channel_list.end()) {
		client->addMessage(handleResponse(client->getNickname(), ERR_NOSUCHCHANNEL, channel_name));
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
		client->addMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "QUIT"));
		return ;
	}
	for (std::size_t i = 0; i < joined_channel.size(); ++i) {
		channel_list[joined_channel[i]]->quit(client, _params[0]);
		if (channel_list[joined_channel[i]]->getUserList().size() == 0)
			server.deleteChannelList(joined_channel[i]);
	}
	client->clearJoinedChannel();
	client->setDisconnect(true);
}

void	Command::privmsg(Server& server, Client* client) {
	//PRIVMSG <receiver>{,<receiver>} <text to be sent>
	std::map<std::string, Channel *> channel_list = server.getChannelList();
	std::vector<std::string> joined_channel = client->getJoinedChannel();

	if (_params.size() < 2) {
		client->addMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "PRIVMSG"));
		return ;
	}
	std::string receiver;
	std::string msg = _params[1];
	std::istringstream ss(_params[0]);
	while (std::getline(ss, receiver, ',')) {
		std::vector<std::string>::iterator it = std::find(joined_channel.begin(), joined_channel.end(), receiver);
		if (receiver[0] == '#') {
			if (channel_list.find(receiver) == channel_list.end())
				client->addMessage(handleResponse(client->getNickname(), ERR_NOSUCHCHANNEL, receiver));
			else if (it == joined_channel.end())
				client->addMessage(handleResponse(client->getNickname(), ERR_CANNOTSENDTOCHAN, receiver));
			else
				channel_list[receiver]->broadcastWithoutClient(messageFormat(PRIVMSG, client, receiver, msg), client);
		} else {
			if (receiver == "bot") {
				excuteBot(client);
				return ;
			}
			Client* receive_client = server.findClient(receiver);
			if (!receive_client)
				client->addMessage(handleResponse(client->getNickname(), ERR_NOSUCHNICK, receiver));
			else
				receive_client->addMessage(messageFormat(PRIVMSG, client, receive_client->getNickname(), msg));
		}
	}
}

void	Command::mode(Server& server, Client* client) {
	//MODE <channel> {[+|-]|i|t|k|o|l} [<limit>] [<user>]
	std::map<std::string, Channel *> channel_list = server.getChannelList();
	if (_params.empty()) {
		client->addMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "MODE"));
		return ;
	}
	if (_params.size() == 1) {
		if (channel_list.find(_params[0]) != channel_list.end()) {
			channel_list[_params[0]]->answerMode(client);
			return ;
		} else {
			client->addMessage(handleResponse(client->getNickname(), ERR_NOSUCHCHANNEL, _params[0]));
			return ;
		}
	}
	std::string	channel_name = _params[0];
	if (channel_list.find(channel_name) == channel_list.end()) {
		client->addMessage(handleResponse(client->getNickname(), ERR_NOSUCHCHANNEL, channel_name));
		return ;
	}
	std::string opt = _params[1];
	std::size_t pos_plus = opt.find('+');
	std::size_t pos_minus = opt.find('-');
	if (pos_plus == std::string::npos && pos_minus == std::string::npos) {
		client->addMessage(handleResponse(client->getNickname(), ERR_UNKNOWNMODE, opt));
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
	std::size_t j = 0;

	std::map<Client *, bool> user_list = channel_list[channel_name]->getUserList();
	if (channel_list[channel_name]->checkChannelMember(client) == false) {
		client->addMessage(handleResponse(client->getNickname(), ERR_NOTONCHANNEL, channel_name));
		return ;
	}
	if (channel_list[channel_name]->checkAuthority(client) == false) {
		client->addMessage(handleResponse(client->getNickname(), ERR_CHANOPRIVSNEEDED, channel_name));
		return ;
	}
	for (std::size_t i = 0; i < opt.size();) {
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
						client->addMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "MODE +k"));
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
						client->addMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "MODE +o"));
						continue ;
					}
					Client* target_client = server.findClient(_params[params_order]);
					if (!target_client) {
						client->addMessage(handleResponse(client->getNickname(), ERR_USERNOTINCHANNEL, _params[params_order], channel_name));
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
					if (opt_l)
						continue ;
					if (params_order == _params.size()) {
						client->addMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "MODE +l"));
						continue ;
					}
					long nb = std::atol(_params[params_order].c_str());
					if (nb <= 0) {
						client->addMessage(handleResponse(client->getNickname(), ERR_UNKNOWNMODE, _params[params_order]));
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
					client->addMessage(handleResponse(client->getNickname(), ERR_UNKNOWNMODE, std::string(1, opt[j])));
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
						client->addMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "MODE -o"));
						continue ;
					}
					Client* target_client = server.findClient(_params[params_order]);
					if (!target_client) {
						client->addMessage(handleResponse(client->getNickname(), ERR_USERNOTINCHANNEL, _params[params_order], channel_name));
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
					client->addMessage(handleResponse(client->getNickname(), ERR_UNKNOWNMODE, std::string(1, opt[j])));
					continue ;
				}
			}
		}
		if (opt[i] != '+' && opt[i] != '-')
			i++;
		else
			i = j;
	}
	reply += params_reply;
	if (!(reply == "+" || reply == "-") && reply.size())
		channel_list[channel_name]->broadcast(messageFormat(MODE, client, channel_name, reply));
}

void	Command::ping(Client* client) {
	if (_params.size() != 1) 
		return ;
	client->addMessage(messageFormat(PONG, client));
}

void	Command::part(Server& server, Client* client) {
	std::map<std::string, Channel *> channel_list = server.getChannelList();
	std::string reason = "";
	if (_params.empty()) {
		client->addMessage(handleResponse(client->getNickname(), ERR_NEEDMOREPARAMS, "PART"));
		return ;
	}
	if (_params.size() >= 2)
		reason = _params[1];
	std::istringstream iss(_params[0]);
	std::string channel_name;
	while (std::getline(iss, channel_name, ',')) {
		if (channel_list.find(channel_name) == channel_list.end()) {
			client->addMessage(handleResponse(client->getNickname(), ERR_NOSUCHCHANNEL, channel_name));
			continue ;
		}
		channel_list[channel_name]->part(client, reason);
		if (channel_list[channel_name]->getUserList().size() == 0)
			server.deleteChannelList(channel_name);
	}
}
