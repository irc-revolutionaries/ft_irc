#include "essential.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

const std::string messageFormat(int cmd_code, Client *client, const std::string& target, \
			const std::string& additionalInfo) {
	std::string	message;

	//:닉네임!유저명@호스트명 PRIVMSG 보내는_채널이름 :전송할_메시지
	std::string nickname = client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname();
	switch (cmd_code)
	{
		case RPL_WELCOME: // 001
            message = ":" + g_server_name + " 001 " + client->getNickname() + " :Welcome to the ft_irc " + nickname + "\r\n";
            break;
        case RPL_YOURHOST: // 002
            message = ":" + g_server_name + " 002 " + client->getNickname() + " :Your host is " + g_server_name + "\r\n";
            break;
        case RPL_CREATED: // 003
            message = ":" + g_server_name + " 003 " + client->getNickname() + " :This server was created " + target + "\r\n";
            break;
        case RPL_MYINFO: // 004
            message = ":" + g_server_name + " 004 " + client->getNickname() + " " + g_server_name + " " + target + "\r\n";
            break;
		case PRIVMSG: //900
			message = ":" + nickname + " PRIVMSG " + target + " " + additionalInfo + "\r\n";
			break;
        case NICK: //901
			message = ":" + nickname + " NICK " + ":" + target + "\r\n";
            break;
        case JOIN: //902
			message = ":" + nickname + " JOIN " + ":" + target + "\r\n";
            break;
        case QUIT: //903
			message = ":" + nickname + " QUIT " +  target + "\r\n";
            break;
		case PONG: //904
			message = "PONG :" + g_server_name + "\r\n";
			break;
		case MODE: //905
			message = ":" + nickname + " MODE " + target + " " + additionalInfo + "\r\n";
			break;
		case PART: //905
			message = ":" + nickname + " PART " + target + " " + additionalInfo + "\r\n";
			break;
	}
	return (message);
}

const std::string handleResponse(const std::string& nickname, int responseCode, \
		const std::string& target, const std::string& additionalInfo) {
	std::string message;
	switch (responseCode) {
	    case ERR_ALREADYREGISTRED: //462
			message = ":" + g_server_name + " 462 " + nickname + " :You may not reregister\r\n";
			break;
	    case ERR_BADCHANMASK: //476
			message = ":" + g_server_name + " 476 " + nickname + " :Bad Channel Mask\r\n";
			break;
		case ERR_BADCHANNELKEY: //475
			message = ":" + g_server_name + " 475 " + nickname + " " + target + " " + ":Cannot join channel (+k)\r\n";
			break;
		case ERR_CANNOTSENDTOCHAN: //404
			message = ":" + g_server_name + " 404 " + nickname + " " + target + " " + ":Cannot send to channel\r\n";
			break;
		case ERR_CHANNELISFULL: //471
	    	message = ":" + g_server_name + " 471 " + nickname + " " + target + " " + ":Cannot join channel (+l)\r\n";
			break;
		case ERR_CHANOPRIVSNEEDED: //482
			message = ":" + g_server_name + " 482 " + target + " " + ":You're not channel operator\r\n";
			break;
        case ERR_ERRONEUSNICKNAME: //432
            message = ":" + g_server_name + " 432 " + nickname + " " + target + " :Erroneous Nickname\r\n";
            break;
		case ERR_INVITEONLYCHAN: // 473
    		message = ":" + g_server_name + " 473 " + nickname + " " + target + " " + ":Cannot join channel (+i)\r\n";
    		break;
		case ERR_KEYSET: // 467
    		message = ":" + g_server_name + " 467 " + nickname + " " + target + " " + ":Channel key already set\r\n";
    		break;
		case ERR_NEEDMOREPARAMS: // 461
		    message = ":" + g_server_name + " 461 " + nickname + " " + target + " " + ":Not enough parameters\r\n";
		    break;
		case ERR_NICKCOLLISION: // 436
		    message = ":" + g_server_name + " 436 " + nickname + " " + target + " " + ":Nickname collision KILL\r\n";
		    break;
		case ERR_NICKNAMEINUSE: // 433
    		message = ":" + g_server_name + " 433 " + nickname + " " + target + " " + ":Nickname is already in use\r\n";
    		break;
		case ERR_NOCHANMODES: // 477
    		message = ":" + g_server_name + " 477 " + nickname + " " + target + " " + ":Channel doesn't support modes\r\n";
    		break;
		case ERR_NONICKNAMEGIVEN: // 431
		    message = ":" + g_server_name + " 431 " + nickname + " " + ":No nickname given\r\n";
		    break;
		case ERR_NORECIPIENT: // 411
		    message = ":" + g_server_name + " 411 " + nickname + " " + ":No recipient given\r\n";
		    break;
		case ERR_NOSUCHCHANNEL: // 403
		    message = ":" + g_server_name + " 403 " + nickname + " " + target + " " + ":No such channel\r\n";
		    break;
		case ERR_NOSUCHNICK: // 401
		    message = ":" + g_server_name + " 401 " + nickname + " " + target + " " + ":No such nick/channel\r\n";
		    break;
		case ERR_NOTEXTTOSEND: // 412
		    message = ":" + g_server_name + " 412 " + nickname + " " + ":No text to send\r\n";
		    break;
		case ERR_NOTONCHANNEL: // 442
    		message = ":" + g_server_name + " 442 " + nickname + " " + target + " " + ":You're not on that channel\r\n";
    		break;
		case ERR_NOTOPLEVEL: // 413
		    message = ":" + g_server_name + " 413 " + nickname + " " + target + " " + ":No toplevel domain specified\r\n";
		    break;
		case ERR_TOOMANYTARGETS: // 407
    		message = ":" + g_server_name + " 407 " + nickname + " " + target + " " + ":Too many recipients. No message delivered\r\n";
    		break;
		case ERR_UNKNOWNMODE: // 472
		    message = ":" + g_server_name + " 472 " + nickname + " " + target + " " + ":Unknown mode char to me\r\n";
		    break;
		case ERR_USERNOTINCHANNEL: // 441
		    message = ":" + g_server_name + " 441 " + nickname + " " + target + " " + additionalInfo + " :They aren't on that channel\r\n";
		    break;
		case ERR_USERONCHANNEL: // 443
    		message = ":" + g_server_name + " 443 " + nickname + " " + target + " " + ":User is already on that channel\r\n";
    		break;
		case RPL_CHANNELMODEIS: // 324
            message = ":" + g_server_name + " 324 " + nickname + " " + target + " " + ":" + additionalInfo + "\r\n";
            break;
        case RPL_INVITING: // 341
            message = ":" + g_server_name + " 341 " + nickname + " " + additionalInfo + " " + target + "\r\n";
            break;
        case RPL_NOTOPIC: // 331
            message = ":" + g_server_name + " 331 " + nickname + " " + target + " " + ":No topic is set\r\n";
            break;
        case RPL_TOPIC: // 332
			// :server 332 <nick> <channel> :<topic>
            message = ":" + g_server_name + " 332 " + nickname + " " + target + " :" + additionalInfo + "\r\n";
            break;
		case RPL_NAMREPLY: //353
            message = ":" + g_server_name + " 353 " + nickname + " = " + target + " :" + additionalInfo + "\r\n";
            break;
        case RPL_ENDOFNAMES: //366
            message = ":" + g_server_name + " 366 " + nickname + " " + target + " " + ":End of /NAMES list\r\n";
            break;
		case ERR_UNKNOWNCOMMAND: //421
            message = ":" + g_server_name + " 421 " + nickname + " " + target + " " + ":Unknown command\r\n";
            break;
        case ERR_NOTREGISTERED: //451
            message = ":" + g_server_name + " 451 " + nickname + " :You have not registered\r\n";
            break;
		case ERR_PASSWDMISMATCH: //464
			message = ":" + g_server_name + " 464 * :Server Password incorrect\r\n";
		case ERR_BADCHANNAME: //479
			message = ":" + g_server_name + " 479 " + nickname + " " + target + " :Channel name contains illegal characters\r\n";
	}
	return (message);
}