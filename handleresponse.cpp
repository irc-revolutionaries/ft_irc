#include "essential.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

const std::string& handleServerResponse(Client *client, int responseCode, \
		const std::string& target = "", const std::string& additionalInfo = "") {
    std::string message;
    switch (responseCode) {
        case 484: // ERR_RESTRICTED
            message = ":" + server_name + " 484 " + client->getNickname() + " :Your connection is restricted\r\n";
            break;
        case 405: // ERR_TOOMANYCHANNELS
            message = ":" + server_name + " 405 " + client->getNickname() + " " + target + " :You have joined too many channels\r\n";
            break;
        case 407: // ERR_TOOMANYTARGETS
            message = ":" + server_name + " 407 " + client->getNickname() + " " + target + " :Duplicate recipients. No message delivered\r\n";
            break;
        case 437: // ERR_UNAVAILRESOURCE
            message = ":" + server_name + " 437 " + client->getNickname() + " " + target + " :Nick/channel is temporarily unavailable\r\n";
            break;
        case 472: // ERR_UNKNOWNMODE
            message = ":" + server_name + " 472 " + client->getNickname() + " " + target + " :Unknown mode char to me\r\n";
            break;
        case 441: // ERR_USERNOTINCHANNEL
            message = ":" + server_name + " 441 " + client->getNickname() + " " + target + " :They aren't on that channel\r\n";
            break;
        case 443: // ERR_USERONCHANNEL
            message = ":" + server_name + " 443 " + client->getNickname() + " " + target + " :User is already on that channel\r\n";
            break;
        case 414: // ERR_WILDTOPLEVEL
            message = ":" + server_name + " 414 " + client->getNickname() + " " + target + " :Wildcard in toplevel domain\r\n";
            break;
        case 301: // RPL_AWAY
            message = ":" + server_name + " 301 " + client->getNickname() + " " + target + " :<away message>\r\n";
            break;
        case 367: // RPL_BANLIST
            message = ":" + server_name + " 367 " + client->getNickname() + " " + target + " :<ban mask>\r\n";
            break;
        case 324: // RPL_CHANNELMODEIS
            message = ":" + server_name + " 324 " + client->getNickname() + " " + target + " :<modes>\r\n";
            break;
        case 368: // RPL_ENDOFBANLIST
            message = ":" + server_name + " 368 " + client->getNickname() + " " + target + " :End of channel ban list\r\n";
            break;
        case 349: // RPL_ENDOFEXCEPTLIST
            message = ":" + server_name + " 349 " + client->getNickname() + " " + target + " :End of channel exception list\r\n";
            break;
        case 347: // RPL_ENDOFINVITELIST
            message = ":" + server_name + " 347 " + client->getNickname() + " " + target + " :End of channel invite list\r\n";
	}
	return (message);
}