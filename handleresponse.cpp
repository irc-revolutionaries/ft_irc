#include "essential.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

const std::string& messageFormat(int cmd_code, Client *client, const std::string& target = "", \
			const std::string& additionalInfo = "") {
	std::string	message;

	//:닉네임!유저명@호스트명 PRIVMSG 보내는_채널이름 :전송할_메시지
	std::string nickname = client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname();
	switch (cmd_code)
	{
	case PRIVMSG:
		message = ":" + nickname + " PRIVMSG " + target + " :" + additionalInfo;
		break;
	case NICK:
		message = ":" + server_name + " 001 " + client->getNickname() + " :Welcome to the ft_irc world," + client->getNickname() + ".";
		break;
	}
	return (message);
}

const std::string& handleResponse(const std::string& nickname, int responseCode, \
		const std::string& target = "", const std::string& additionalInfo = "") {
	std::string message;
	switch (responseCode) {
	    case ERR_ALREADYREGISTRED: //462
			message = ":" + server_name + " 462 " + nickname + " :You may not reregister\r\n";
			break;
	    case ERR_BADCHANMASK: //476
			message = ":" + server_name + " 476 " + nickname + " :Bad Channel Mask\r\n";
			break;
		case ERR_BADCHANNELKEY: //475
			message = ":" + server_name + " 475 " + nickname + " " + target + " " + ":Cannot join channel (+k)\r\n";
			break;
		case ERR_CANNOTSENDTOCHAN: //404
			message = ":" + server_name + " 404 " + nickname + " " + target + " " + ":Cannot send to channel\r\n";
			break;
		case ERR_CHANNELISFULL: //471
	    	message = ":" + server_name + " 471 " + nickname + " " + target + " " + ":Cannot join channel (+l)\r\n";
			break;
		case ERR_CHANOPRIVSNEEDED: //482
			message = ":" + server_name + " 482 " + nickname + " " + target + " " + ":You're not channel operator\r\n";
			break;
        case ERR_ERRONEUSNICKNAME: //432
            message = ":" + server_name + " 432 " + nickname + " " + target + " :Erroneous Nickname\r\n";
            break;
		case ERR_INVITEONLYCHAN: // 473
    		message = ":" + server_name + " 473 " + nickname + " " + target + " " + ":Cannot join channel (+i)\r\n";
    		break;
		case ERR_KEYSET: // 467
    		message = ":" + server_name + " 467 " + nickname + " " + target + " " + ":Channel key already set\r\n";
    		break;
		case ERR_NEEDMOREPARAMS: // 461
		    message = ":" + server_name + " 461 " + nickname + " " + target + " " + ":Not enough parameters\r\n";
		    break;
		case ERR_NICKCOLLISION: // 436
		    message = ":" + server_name + " 436 " + nickname + " " + target + " " + ":Nickname collision KILL\r\n";
		    break;
		case ERR_NICKNAMEINUSE: // 433
    		message = ":" + server_name + " 433 " + nickname + " " + target + " " + ":Nickname is already in use\r\n";
    		break;
		case ERR_NOCHANMODES: // 477
    		message = ":" + server_name + " 477 " + nickname + " " + target + " " + ":Channel doesn't support modes\r\n";
    		break;
		case ERR_NONICKNAMEGIVEN: // 431
		    message = ":" + server_name + " 431 " + nickname + " " + ":No nickname given\r\n";
		    break;
		case ERR_NORECIPIENT: // 411
		    message = ":" + server_name + " 411 " + nickname + " " + ":No recipient given\r\n";
		    break;
		case ERR_NOSUCHCHANNEL: // 403
		    message = ":" + server_name + " 403 " + nickname + " " + target + " " + ":No such channel\r\n";
		    break;
		case ERR_NOSUCHNICK: // 401
		    message = ":" + server_name + " 401 " + nickname + " " + target + " " + ":No such nick/channel\r\n";
		    break;
		case ERR_NOTEXTTOSEND: // 412
		    message = ":" + server_name + " 412 " + nickname + " " + ":No text to send\r\n";
		    break;
		case ERR_NOTONCHANNEL: // 442
    		message = ":" + server_name + " 442 " + nickname + " " + target + " " + ":You're not on that channel\r\n";
    		break;
		case ERR_NOTOPLEVEL: // 413
		    message = ":" + server_name + " 413 " + nickname + " " + target + " " + ":No toplevel domain specified\r\n";
		    break;
		case ERR_TOOMANYTARGETS: // 407
    		message = ":" + server_name + " 407 " + nickname + " " + target + " " + ":Too many recipients. No message delivered\r\n";
    		break;
		case ERR_UNKNOWNMODE: // 472
		    message = ":" + server_name + " 472 " + nickname + " " + target + " " + ":Unknown mode char to me\r\n";
		    break;
		case ERR_USERNOTINCHANNEL: // 441
		    message = ":" + server_name + " 441 " + nickname + " " + target + " " + ":They aren't on that channel\r\n";
		    break;
		case ERR_USERONCHANNEL: // 443
    		message = ":" + server_name + " 443 " + nickname + " " + target + " " + ":User is already on that channel\r\n";
    		break;
		case RPL_CHANNELMODEIS: // 324
            message = ":" + server_name + " 324 " + nickname + " " + target + " " + ":Channel mode is " + additionalInfo + "\r\n";
            break;
        case RPL_INVITING: // 341
            message = ":" + server_name + " 341 " + nickname + " " + target + " " + ":Inviting\r\n";
            break;
        case RPL_NOTOPIC: // 331
            message = ":" + server_name + " 331 " + nickname + " " + target + " " + ":No topic is set\r\n";
            break;
        case RPL_TOPIC: // 332
            message = ":" + server_name + " 332 " + nickname + " " + target + " " + additionalInfo + " " + ":Topic\r\n";
            break;
	}
	return (message);
}