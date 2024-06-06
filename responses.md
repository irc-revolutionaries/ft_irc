# irc responses
## [파싱]
- ERR_NEEDMOREPARAMS	: 매개변수 부족
- ERR_BADCHANMASK		: 채널 이름 잘못됨
- ERR_NOSUCHCHANNEL		: 해당 채널 못 찾음
- ERR_TOOMANYTARGETS	: 한번에 많은 사용자에게 메세지를 보냄
- ERR_NOSUCHNICK		: 클라이언트 찾지 못함

## [channel-JOIN]
- ERR_INVITEONLYCHAN : +i 옵션 채널 join 실
- ERR_BADCHANNELKEY	 : +k 옵션 채널 join 실패
- ERR_CHANNELISFULL	 : +ㅣ 옵션 채널 join 실패
- RPL_TOPIC	: 채널의 topic에 대한 요청을 받을 때 발생

## [channel-INVITE]
- ERR_NOTONCHANNEL		: request 사용자가 채널에 있지 않을 때
- ERR_CHANOPRIVSNEEDED	: request 사용자가 operator가 아닐 때
- ERR_USERONCHANNEL		: target 사용자가 이미 채널에 있을 때
- RPL_INVITING	: 다른 사용자를 초대할 때 발생

## [channel-KICK]
- ERR_NOTONCHANNEL		: request 사용자가 채널에 있지 않을 때
- ERR_CHANOPRIVSNEEDED	: request 사용자가 operator가 아닐 때
- ERR_USERNOTINCHANNEL	: target 사용자가 채널에 있지 않을 때

## [channel-TOPIC]
- ERR_NOTONCHANNEL	: request 사용자가 채널에 있지 않을 때
- ERR_NOTONCHANNEL	: request 사용자가 채널에 있지 않을 때
- RPL_NOTOPIC		: topic이 없을 때
- RPL_TOPIC			: topic 반환될 때

## [channel-PART]
- ERR_NOTONCHANNEL	: request 사용자가 채널에 있지 않을 때