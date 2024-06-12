#include "essential.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Command.hpp"

int	main(int argc, char **argv) {
	//argv[1] == port, argv[2] == password
	if (argc != 3)
		exitMessage("Input format : [PORT] [PASSWORD]\n");
	Server server(argv[1], argv[2]);
	std::vector<struct kevent>	change_list;
	struct kevent	event_list[EVENT_MAX];
	struct kevent*	curr_event;
	int	new_events;

	server.setServer(change_list);
	signal(SIGPIPE, SIG_IGN);
	while (true) {
		new_events = kevent(int(server.getKq()), &change_list[0], change_list.size(), \
				event_list, EVENT_MAX, NULL);
		if (new_events == -1)
			exitMessage("kevent error");
		change_list.clear();

		for (int i = 0; i < new_events; ++i) {
			curr_event = &event_list[i];
			if (curr_event->flags & EV_ERROR) {
				if (curr_event->ident == (unsigned long)(server.getFd()))
					exitMessage("server socket error");
				else {
					std::cerr << "client socket error" << "\n";
					server.disconnectClient(curr_event->ident);
				}
			} else if (curr_event->filter == EVFILT_READ) {
				if (curr_event->ident == (unsigned long)(server.getFd()))
					server.addClient(change_list);
				else if (curr_event->flags & EV_EOF)
					server.disconnectClient(curr_event->ident);
				else if (server.getClientList().find(curr_event->ident) != \
							server.getClientList().end())
					server.makeCommand(curr_event->ident);
			} else if (curr_event->filter == EVFILT_WRITE)
				server.sendMessage(curr_event->ident);
		}
	}
}