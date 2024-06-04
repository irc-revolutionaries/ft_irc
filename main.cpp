#include "essential.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Command.hpp"

int	main(int argc, char **argv) {
//argv[1] == port, argv[2] == password
	Server server(argv[1], argv[2]);
	Command cmd;
	std::vector<struct kevent>	change_list;
	struct kevent*	curr_event;
	struct kevent	event_list[EVENT_MAX];
	int	new_events;

	server.setServer(change_list);
	while (true) {
		new_events = kevent(server.getFd(), &change_list[0], change_list.size(), 
				event_list, EVENT_MAX, NULL);
		change_list.clear();

		for (int i = 0; i < new_events; ++i) {
			curr_event = &event_list[i];
			if (curr_event->flags & EV_ERROR) {
				if (curr_event->ident == server.getFd())
					exitMsg("server socket error");
				else {
					std::cerr << "client socket error" << "\n";
					server.disconnectClient(curr_event->ident);
				}
			} else if (curr_event->filter == EVFILT_READ) {
				if (curr_event->ident == server.getFd())
					server.addClient(change_list);
				else if (server.getClientList().find(curr_event->ident) != \
							server.getClientList().end())
					server.makeCommand(curr_event->ident);
			} else if (curr_event->filter == EVFILT_WRITE) {
				std::map<int, Client*>::const_iterator it = server.getClientList().find(static_cast<int>(curr_event->ident));
				if (it != server.getClientList().end()) {
					std::vector<std::string> msg_vec = it->second->getMessage();
					for (int i = 0; i < msg_vec.size(); ++i) {
						ssize_t	n = send(curr_event->ident, msg_vec[i].c_str(), msg_vec[i].length(), 0);
						if (n < 0)
							exitMsg("send error");
					}
				}
			}
		}
	}
}