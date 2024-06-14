#include "Bonus.hpp"
#include "Client.hpp"

void excuteBot(Client* request_client) {
	request_client->addMessage(":bot!bot@" + request_client->getHostname() + " PRIVMSG " + request_client->getNickname() + " :아이앰 그루트\r\n");
}