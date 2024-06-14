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
	signal(SIGPIPE, SIG_IGN);
	server.startServer();
}
