#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Command.hpp"

std::string g_server_name;

Server::Server(const char* port, const char* password) {
	std::string str = port;

	for (int i = 0; port[i]; ++i)
		if (!std::isdigit(port[i]))
			exitMessage("port number error");
	if (str.size() > 5)
		exitMessage("Port number error\n Please input 1024 ~ 49151");
	_port = std::atoi(port);
	if (_port < 1024 || _port > 49151)
		exitMessage("Port number error\n Please input 1024 ~ 49151");
	_password = password;
	_name = "ft_irc_server";
	g_server_name = _name;
}

Server::~Server() {
	std::map<int, Client *>::iterator it;

	for (it = _client_list.begin(); it != _client_list.end(); it++) {
		close(it->first);
		delete _client_list[it->first];
	}
	close(_fd);
}

const std::map<std::string, Channel *>&	Server::getChannelList() const { return (_channel_list); }
const std::map<int, Client *>&			Server::getClientList() const { return (_client_list); }
const std::string&	Server::getPassword() const { return (_password); }
std::size_t	Server::getFd() const { return (_fd); }
int		Server::getPort() const { return (_port); }
int		Server::getKq() const { return (_kq); }

//서버 소켓 설정
void	Server::setServer() {
	int	option = 1;

	//소켓 생성
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (int(_fd) == -1)
		exitMessage("Socket error");
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (void *)(&option), sizeof(option));
	
	memset(&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_server_addr.sin_port = htons(_port);
	//소켓에 주소 할당
	if (bind(_fd, (struct sockaddr *)(&_server_addr), sizeof(_server_addr)) == -1)
		exitMessage("bind error");
	
	//소켓을 수신 대기 상대로 만들기
	if (listen(_fd, 5) == -1)
		exitMessage("listen error");
	
	if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1) //소켓 non-blocking 설정
		exitMessage("Server Socket fcntl error");
	
	//kqueue 생성
	_kq = kqueue();
	if (_kq == -1)
		exitMessage("kqueue error");
	//kevent 저장 벡터, 이벤트를 감시할 식별자, 이벤트 필터, 이벤트 플래그(새로운 이벤트 추가, 이벤트 활성화)
	changeEvents(_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
}

void	Server::addClient() {
	int	client_fd;

	//클라이언트 소켓 연결
	client_fd = accept(_fd, NULL, NULL);
	if (client_fd == -1) 
		exitMessage("Client accept error");
	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "Client Socket fcntl error\n";
		close(client_fd);
		return ;
	}
		
	changeEvents(client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	changeEvents(client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

	//클라이언트 추가
	Client *new_client = new Client(client_fd);
	_client_list[client_fd] = new_client;
}

void    Server::makeCommand(int ident) {
    Command cmd;
    char    buf[MAX_BUF];
    ssize_t n = recv(ident, buf, MAX_BUF, 0); //메세지 수신
	Client *client = _client_list[ident];

    if (n <= 0) {
		if (n < 0)
        	std::cerr << "client read error\n";
		else
			std::cout << "client EOF\n";
		disconnectClient(ident);
	} else {
		if (n <= 512)
			buf[n - 1] = 0;
		else
			buf[n] = 0;
		client->setCommand(client->getCommand() + buf);
        if (client->getCommand().find('\n') != std::string::npos ||\
			client->getCommand().find('\r') != std::string::npos) {
			char find_char = (client->getCommand().find('\n') != std::string::npos) ? '\n' : '\r';
			char del_char = (find_char == '\n') ? '\r' : '\n';
			std::istringstream iss(client->getCommand());
			std::string tmp;
			if (client->getCommand().find(find_char) != std::string::npos) {
				while (std::getline(iss, tmp, find_char)) {
					while (tmp.find(del_char) != std::string::npos)
						tmp.replace(tmp.find(del_char), 1, "");
					std::cout << "\nCommand : " << tmp << "\n";
					cmd.handleCmd(*this, _client_list[ident], tmp);
				}
			}
            client->setCommand("");
        }
    }
}

void	Server::sendMessage(int ident) {
	std::map<int, Client *>::const_iterator it = _client_list.find(ident);
	if (it != _client_list.end()) {
		std::vector<std::string> msg_vec = it->second->getMessage();
		for (std::size_t i  = 0; i < msg_vec.size(); ++i) {
			std::size_t send_size = 0;
			ssize_t n = 0;
			while (send_size < msg_vec[i].length()) {
				n = send(ident, msg_vec[i].c_str() + send_size, msg_vec[i].length(), 0);
				if (n < 0) {
					disconnectClient(ident);
					return ;
				}
				send_size += n;
			}
			std::cout << "Send message : " << msg_vec[i];
		}
		it->second->clearMessage();
		if (it->second->getDisconnect() == true) {
			disconnectClient(ident);
			return ;
		}
	}
}

void	Server::disconnectClient(int client_fd) {
	const std::vector<std::string>& joined_channel = _client_list[client_fd]->getJoinedChannel();
	std::map<std::string, Channel *>::iterator it;

	std::cout << "client disconnected: " << client_fd << "\n";
	close(client_fd); //연결 종료
	for (it = _channel_list.begin(); it != _channel_list.end(); ++it) 
		_channel_list[it->first]->deleteInviteList(_client_list[client_fd]->getNickname());
	for (std::size_t i = 0; i < joined_channel.size(); ++i) {
		_channel_list[joined_channel[i]]->errorQuit(_client_list[client_fd]);
		if (_channel_list[joined_channel[i]]->getUserList().size() == 0) {
			deleteChannelList(joined_channel[i]);
		}
	}
	delete _client_list[client_fd];
	_client_list.erase(client_fd);
}

void	Server::createChannel(std::string ch_name) {
	std::pair<std::string, Channel *> channel_arg;
	channel_arg.first = ch_name;
	channel_arg.second = new Channel(ch_name);
	_channel_list.insert(channel_arg);
}

void	Server::deleteChannelList(std::string ch_name) {
	delete _channel_list.find(ch_name)->second;
	_channel_list.erase(ch_name);
}

//kevent 구조체 세팅, 감지 이벤트 리스트 추가
void	Server::changeEvents(uintptr_t ident, int16_t filter, \
			uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
	struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    _change_list.push_back(temp_event);
}

//클라이언트의 nickname으로 find
Client* Server::findClient(const std::string& name) {
	std::map<int, Client *>::iterator it;
	for (it = _client_list.begin(); it != _client_list.end(); it++)
		if (it->second->getNickname() == name)
			return (it->second);
	return (NULL);
}

void	Server::startServer() {
	struct kevent	event_list[EVENT_MAX];
	struct kevent*	curr_event;
	int	new_events;

	this->setServer();
	std::cout << "Server start\n";
	while (true) {
		new_events = kevent(_kq, &_change_list[0], _change_list.size(), event_list, EVENT_MAX, NULL);
		if (new_events == -1)
			exitMessage("kevent error");
		_change_list.clear();

		for (int i = 0; i < new_events; ++i) {
			curr_event = &event_list[i];
			if (curr_event->flags & EV_ERROR) {
				if (curr_event->ident == (unsigned long)(_fd))
					exitMessage("server socket error");
				else {
					std::cerr << "client socket error" << "\n";
					this->disconnectClient(curr_event->ident);
				}
			} else if (curr_event->filter == EVFILT_READ) {
				if (curr_event->ident == (unsigned long)(_fd))
					this->addClient();
				else if (_client_list.find(curr_event->ident) != _client_list.end())
					this->makeCommand(curr_event->ident);
			} else if (curr_event->filter == EVFILT_WRITE)
				this->sendMessage(curr_event->ident);
		}
	}
}

void	exitMessage(const std::string& msg) {
	std::cerr << msg << "\n";
	std::exit(1);
}
