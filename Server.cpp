#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Command.hpp"

Server::Server(const char* port, const char* password) {
	for (int i = 0; port[i]; ++i)
		if (!isdigit(port[i]))
			exitMessage("port number error");
	_port = atoi(port);
	_password = password;
	_name = "ft_irc_server";
	server_name = _name;
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
size_t	Server::getFd() const { return (_fd); }
int		Server::getPort() const { return (_port); }
int		Server::getKq() const { return (_kq); }

//서버 소켓 설정
void	Server::setServer(std::vector<struct kevent>& change_list) {
	//소켓 생성
	_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (int(_fd) == -1)
		exitMessage("socket error\n" + std::string(strerror(errno)));
	
	memset(&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_server_addr.sin_port = htons(_port);
	//소켓에 주소 할당
	if (bind(_fd, (struct sockaddr *)(&_server_addr), sizeof(_server_addr)) == -1)
		exitMessage("bind error");
	
	//소켓을 수신 대기 상대로 만들기
	if (listen(_fd, 10) == -1)
		exitMessage("listen error");
	fcntl(_fd, F_SETFL, O_NONBLOCK); //소켓 non-blocking 설정
	
	//kqueue 생성
	_kq = kqueue();
	if (_kq == -1)
		exitMessage("kqueue error");
	//kevent 저장 벡터, 이벤트를 감시할 식별자, 이벤트 필터, 이벤트 플래그(새로운 이벤트 추가, 이벤트 활성화)
	changeEvents(change_list, _fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
}

void	Server::addClient(std::vector<struct kevent>& change_list) {
	int	client_fd;

	//클라이언트 소켓 연결
	client_fd = accept(_fd, NULL, NULL);
	if (client_fd == -1)
		exitMessage("accept error");
	std::cout << "accept new clinet: " << client_fd << "\n";
	fcntl(client_fd, F_SETFL, O_NONBLOCK); //non-blocking 모드
	changeEvents(change_list, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	changeEvents(change_list, client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

	//클라이언트 추가
	Client *new_client = new Client(client_fd);
	_client_list[client_fd] = new_client;
}

void	Server::makeCommand(int ident) {
	Command	cmd;
	char	buf[MAX_BUF];
	ssize_t	n = recv(ident, buf, MAX_BUF, 0); //메세지 수신

	if (n < 0)
		std::cerr << "client read error\n";
	else {
		buf[n] = '\0';
		_command += buf;
		if (_command.find('\n') != 0 || _command.find('\r')) {
			cmd.handleCmd(*this, _client_list[ident], _command);
			_command = "";
		}
	}
}

void	Server::sendMessage(int ident) {
	std::map<int, Client *>::const_iterator it = _client_list.find(ident);
	if (it != _client_list.end()) {
		std::vector<std::string> msg_vec = it->second->getMessage();
		for (size_t i  = 0; i < msg_vec.size(); ++i) {
			ssize_t	n = send(ident, (msg_vec[i] + "\n\r").c_str(), msg_vec[i].length(), 0);
			if (n < 0)
				exitMessage("send error");
		}
	}
}

void	Server::disconnectClient(int client_fd) {
	std::vector<std::string> joined_channel = _client_list[client_fd]->getJoinedChannel();

	std::cout << "client disconnected: " << client_fd << "\n";
	close(client_fd); //연결 종료
	for (size_t i = 0; i < joined_channel.size(); ++i) {
		_channel_list[joined_channel[i]]->errorQuit(_client_list[client_fd]);
		if (_channel_list[joined_channel[i]]->getUserList().size() == 0) {
			delete _channel_list[joined_channel[i]];
			_channel_list.erase(joined_channel[i]);
		}
	}
	delete _client_list[client_fd];
	_client_list.erase(client_fd);
}

void	Server::createChannel(Client *first_client, std::string ch_name) {
	std::pair<std::string, Channel *> channel_arg;
	channel_arg.first = ch_name;
	channel_arg.second = new Channel(ch_name, first_client);
	_channel_list.insert(channel_arg);
}

//클라이언트의 nickname으로 find
Client* Server::findClient(const std::string& name) {
	std::map<int, Client *>::iterator it;
	for (it = _client_list.begin(); it != _client_list.end(); it++)
		if (it->second->getNickname() == name)
			return (it->second);
	return (NULL);
}

void	exitMessage(const std::string& msg) {
	std::cerr << msg << "\n";
	exit(EXIT_FAILURE);
}

//kevent 구조체 세팅, 감지 이벤트 리스트 추가
void changeEvents(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    change_list.push_back(temp_event);	
}

void	Server::deleteChannelList(std::string ch_name) {
	_channel_list.erase(ch_name);
}
