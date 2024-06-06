#include <sys/types.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

#define EVENT_MAX	64
#define MAX_BUF		512

//Error replies
#define ERR_ALREADYREGISTRED  462
#define ERR_BADCHANMASK       476
#define ERR_BADCHANNELKEY     475
#define ERR_BANNEDFROMCHAN    474
#define ERR_CANNOTSENDTOCHAN  404
#define ERR_CHANNELISFULL     471
#define ERR_CHANOPRIVSNEEDED  482
#define ERR_ERRONEUSNICKNAME  432
#define ERR_INVITEONLYCHAN    473
#define ERR_KEYSET            467
#define ERR_NEEDMOREPARAMS    461
#define ERR_NICKCOLLISION     436
#define ERR_NICKNAMEINUSE     433
#define ERR_NOCHANMODES       477
#define ERR_NONICKNAMEGIVEN   431
#define ERR_NORECIPIENT       411
#define ERR_NOSUCHCHANNEL     403
#define ERR_NOSUCHNICK        401
#define ERR_NOTEXTTOSEND      412
#define ERR_NOTONCHANNEL      442
#define ERR_NOTOPLEVEL        413
#define ERR_TOOMANYTARGETS    407
#define ERR_UNKNOWNMODE       472
#define ERR_USERNOTINCHANNEL  441
#define ERR_USERONCHANNEL     443

//Command responses
#define RPL_CHANNELMODEIS     324
#define RPL_INVITING          341
#define RPL_NOTOPIC           331
#define RPL_TOPIC             332