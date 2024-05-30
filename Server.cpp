#include <sys/types.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <vector>

void	exit_with_perror(const std::string& msg)
{
	std::cerr << msg << "\n";
	exit(EXIT_FAILURE);
}

void	disconnect_client(int client_fd, std::map<int, std::string>& clients)
{
	std::cout << "client disconnected: " << client_fd << "\n";
	close(client_fd);
	clients.erase(client_fd);
}

void change_events(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
    struct kevent temp_event;

	//EV_SET 매크로 : kevent구조체의 필드를 설정하는 편리한 방법을 제공
    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    change_list.push_back(temp_event);
}

int	main()
{
	int	server_socket;
	struct sockaddr_in server_addr;

	//PF_INET: IPv4 통신 영역, SOCK_STREAM: 연결지향형 소켓
	//0: 앞선 2개의 내용만으로 충분(프로토콜 정보를 좀 더 구체화 할 필요가 있을 때 설정)  
	if ((server_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        exit_with_perror("socket() error\n" + std::string(strerror(errno)));

	memset(&server_addr, 0, sizeof(server_addr));
	//프로토콜 체계에 저장할 주소 체계 AF_INET : IPv4
	server_addr.sin_family = AF_INET;
	//32비트 IP주소 저장. 네트워크 바이트 순서로 변환
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//16비트 포트번호 저장. 네트워크 바이트 순서로 변환
	server_addr.sin_port = htons(8080);
	//server_socket에 server_addr 주소(로컬 주소) 할당
	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        exit_with_perror("bind() error\n" + std::string(strerror(errno)));

	//소켓을 수신 대기 상태로 만듬, 뒤에 5는 동시에 대기열에 쌓일 수 있는 최대 연결 요청 수를 의미
    if (listen(server_socket, 5) == -1)
        exit_with_perror("listen() error\n" + std::string(strerror(errno)));
	fcntl(server_socket, F_SETFL, O_NONBLOCK);

	int kq;
	if ((kq = kqueue()) == -1)
        exit_with_perror("kqueue() error\n" + std::string(strerror(errno)));

	std::map<int, std::string> clients;
	std::vector<struct kevent> change_list;
	struct kevent event_list[8];

	//change_list : kevent 구조체 저장 벡터 , server_socket : 이벤트를 감시할 식별자
	//EVFILT_READ : 이벤트 필터. 읽기 이벤트를 감시. 즉, 소켓에 읽을 데이터가 있을때 이벤트 발생
	//EV_ADD | EV_ENABLE : 이벤트 플래그. 각각 새로운 이벤트 추가, 이벤트 활성화 기능
	//NULL : 사용자 데이터 저장 안함
	change_events(change_list, server_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	std::cout << "echo server started\n";

	int new_events;
	struct kevent* curr_event;
	while (1)
	{
		//kq : kqueue의 fd, &change_list[0] : 이벤트 변경 목록의 첫 번째 요소의 주소. 이벤트 변경 요청을 나타냄
		//change_list.size() : 변경 목록에 있는 이벤트의 수
		//event_list : 이벤트 목록을 저장할 kevent구조체 배열의 주소. 이 배열은 발생한 이벤트에 대한 정보를 담음
		//8 : event_list 배열의 크기로, 함수가 반환할 수 있는 최대 이벤트 수
		//NULL : 타임아웃 정보를 설정하는 구조체의 포인터. NULL이면 즉시 반환하며 이벤트가 없으면 0을 반환
		new_events = kevent(kq, &change_list[0], change_list.size(), event_list, 8, NULL);
		change_list.clear();

		for (int i = 0; i < new_events; ++i)
		{
			curr_event = &event_list[i];

			if (curr_event->flags & EV_ERROR) //이벤트 오류 처리
			{
				if (curr_event->ident == server_socket)
					exit_with_perror("server socket error");
				else
				{
					std::cerr << "client socket error" << "\n";
					disconnect_client(curr_event->ident, clients);
				}
			}
			else if (curr_event->filter == EVFILT_READ) //읽기 이벤트 처리
			{
				if (curr_event->ident == server_socket) //새로운 클라이언트의 서버 연결 요청
				{
					int client_socket;
					//server_socket: 서버 소켓의 파일 디스크립터. listen함수로 대기중인 소켓을 가리킴
					//NULL: 연결된 클라이언트의 address 정보를 저장할 sockaddr구조체에 대한 포인터
					//NULL: addr구조체의 
					if ((client_socket = accept(server_socket, NULL, NULL)) == -1)
						exit_with_perror("accept error\n" + std::string(strerror(errno)));
					std::cout << "accept new client: " << client_socket << std::endl;
					fcntl(client_socket, F_SETFL, O_NONBLOCK);

					change_events(change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
					change_events(change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
					clients[client_socket] = "";
				}
				else if (clients.find(curr_event->ident) != clients.end()) //read data
				{
					char buf[1024];
					int n = read(curr_event->ident, buf, sizeof(buf));

					if (n <= 0)
					{
						if (n < 0)
							std::cerr << "client read error!" << std::endl;
						disconnect_client(curr_event->ident, clients);
					}
					else
					{
						buf[n] = '\0';
						clients[curr_event->ident] += buf;
						std::cout << "received data from " << curr_event->ident << ": "
								<< clients[curr_event->ident] << std::endl;
					}
				}
			}
			else if (curr_event->filter == EVFILT_WRITE) //write(send) data
			{
				std::map<int, std::string>::iterator it = clients.find(curr_event->ident);
				if (it != clients.end())
				{
					if (clients[curr_event->ident] != "")
					{
						int n;
						if ((n = write(curr_event->ident, clients[curr_event->ident].c_str(),
								clients[curr_event->ident].size()) == -1))
						{
							std::cerr << "client write error!" << std::endl;
							disconnect_client(curr_event->ident, clients);
						}
						else
							clients[curr_event->ident].clear();
					}
				}
			}
		}
	}
}