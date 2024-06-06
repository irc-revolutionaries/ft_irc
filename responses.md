# irc responses
## [파싱]
- ERR_NEEDMOREPARAMS	: 매개변수 부족
- ERR_BADCHANMASK		: 채널 이름 잘못됨
- ERR_NOSUCHCHANNEL		: 해당 채널 못 찾음
- ERR_TOOMANYTARGETS	: 한번에 많은 사용자에게 메세지를 보냄
- ERR_NOSUCHNICK		: 클라이언트 찾지 못함
- ERR_UNKNOWNMODE		: itkol 이외의 옵션은 찾지 못함

## [channel-JOIN] - OK
- ERR_INVITEONLYCHAN : +i 옵션 채널 join 실패 - OK
- ERR_BADCHANNELKEY	 : +k 옵션 채널 join 실패 - OK
- ERR_CHANNELISFULL	 : +ㅣ 옵션 채널 join 실패 - OK
- RPL_TOPIC	: 채널의 topic에 대한 요청을 받을 때 발생 - OK

## [channel-INVITE] - OK
- ERR_NOTONCHANNEL		: request 사용자가 채널에 있지 않을 때 - OK
- ERR_CHANOPRIVSNEEDED	: request 사용자가 operator가 아닐 때 - OK
- ERR_USERONCHANNEL		: target 사용자가 이미 채널에 있을 때 - OK
- RPL_INVITING	: 다른 사용자를 초대할 때 발생 - OK

## [channel-KICK] - OK
- ERR_NOTONCHANNEL		: request 사용자가 채널에 있지 않을 때 - OK
- ERR_CHANOPRIVSNEEDED	: request 사용자가 operator가 아닐 때 - OK
- ERR_USERNOTINCHANNEL	: target 사용자가 채널에 있지 않을 때 - OK

## [channel-TOPIC] - OK
- ERR_NOTONCHANNEL		: request 사용자가 채널에 있지 않을 때 - OK
- ERR_CHANOPRIVSNEEDED	: request 사용자가 operator가 아닐 때 - OK
- RPL_NOTOPIC			: topic이 없을 때 - OK
- RPL_TOPIC				: topic 반환될 때 - OK

## [channel-MODE]
- ERR_KEYSET			: key가 이미 설정되어 있음 (+k 옵션) - OK
- ERR_NOTONCHANNEL		: request 사용자가 채널에 있지 않을 때 - OK
- ERR_CHANOPRIVSNEEDED	: request 사용자가 operator가 아닐 때 - OK
- ERR_USERNOTINCHANNEL	: target 사용자가 채널에 있지 않을 때 - OK
- RPL_CHANNELMODEIS		: mode 반환될 때