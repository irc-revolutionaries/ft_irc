#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Command.hpp"

Server::Server(const char* port, const char* password) {
	for (int i = 0; port[i]; ++i)
		if (!isdigit(port[i]))
			exitMsg("port number error");
	_port = atoi(port);
	_password = password;
}

Server::~Server() {
	std::map<int, Client *>::iterator it;
	for (it = _client_list.begin(); it != _client_list.end(); it++) {
		close(it->first);
		delete _client_list[it->first];
	}
}

const std::map<std::string, Channel *>&	Server::getChannelList() const { return (_channel_list); }
const std::map<int, Client *>&			Server::getClientList() const { return (_client_list); }
const std::string&	Server::getPassword() const { return (_password); }
const int Server::getPort() const { return (_port); }
const int Server::getFd() const { return (_fd); }
const int Server::getKq() const { return (_kq); }

void	Server::setServer(std::vector<struct kevent>& change_list) {
	_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (_fd == -1)
		exitMsg("socket error\n" + std::string(strerror(errno)));
	
	memset(&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_server_addr.sin_port = htons(_port);
	if (bind(_fd, (struct sockaddr *)(&_server_addr), sizeof(_server_addr)) == -1)
		exitMsg("bind error\n" + std::string(strerror(errno)));
	
	if (listen(_fd, 10) == -1) //최대연결 요청 수를 몇으로 해야할 지..?
		exitMsg("listen error\n" + std::string(strerror(errno)));
	fcntl(_fd, F_SETFL, O_NONBLOCK);
	
	_kq = kqueue();
	if (_kq == -1)
		exitMsg("kqueue error\n" + std::string(strerror(errno)));
	changeEvents(change_list, _fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
}

void	Server::addClient(std::vector<struct kevent>& change_list) {
	int	client_fd;

	client_fd == accept(_fd, NULL, NULL);
	if (client_fd == -1)
		exitMsg("accept error\n" + std::string(strerror(errno)));
	std::cout << "accept new clinet: " << client_fd << "\n";
	fcntl(client_fd, F_SETFL, O_NONBLOCK);
	changeEvents(change_list, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	changeEvents(change_list, client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

	Client *new_client = new Client(client_fd);
	_client_list[client_fd] = new_client;
}

void	Server::makeCommand(int ident) {
	Command	cmd;
	char	buf[MAX_BUF];
	ssize_t	n = recv(ident, buf, MAX_BUF, 0);

	if (n <= 0) {
		if (n < 0)
			std::cerr << "client read error\n";
		//eof
	} else {
		buf[n] = '\0';
		cmd.handleCmd(*this, _client_list[ident], buf);
	}
}

void	Server::disconnectClient(int client_fd) {
	std::cout << "client disconnected: " << client_fd << "\n";
	close(client_fd);
	delete _client_list[client_fd];
	_client_list.erase(client_fd);
}

void	Server::createChannel(Client *first_client, std::string ch_name) {
	std::pair<std::string, Channel *> channel_arg;
	channel_arg.first = ch_name;
	channel_arg.second = new Channel(ch_name, first_client);
	_channel_list.insert(channel_arg);
}

Client* Server::findClient(const std::string& name) {
	std::map<int, Client *>::iterator it;
	for (it = _client_list.begin(); it != _client_list.end(); it++)
		if (it->second->getNickname() == name)
			return (it->second);
	return (NULL);
}

void	exitMsg(const std::string& msg) {
	std::cerr << msg << "\n";
	exit(EXIT_FAILURE);
}

void changeEvents(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    change_list.push_back(temp_event);	
}
