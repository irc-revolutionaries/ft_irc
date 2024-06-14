#pragma once

#include "essential.hpp"

class Client {
public :
	Client(int fd);
	
	void	setNickname(const std::string& nickname);
	void	setUsername(const std::string& username);
	void	setRealname(const std::string& realname);
	void	setHostname(const std::string& hostname);
	void	setCommand(const std::string& command);
	void	setServername(const std::string& servername);
	void	setPass(bool check);
	void	setNick(bool check);
	void	setUser(bool check);
	void	setAllReady(bool check);
	void	setDisconnect(bool check);
	
	bool	getPass() const;
	bool	getNick() const;
	bool	getUser() const;
	bool	getAllReady() const;
	bool	getDisconnect() const;

	int	getFd() const;
	const std::string&	getNickname() const;
	const std::string&	getUsername() const;
	const std::string&	getRealname() const;
	const std::string&	getHostname() const;
	const std::string&	getServername() const;
	const std::string&	getCommand() const;
	const std::vector<std::string>&	getMessage() const;

	void	addMessage(const std::string& message);
	void	clearMessage();
	const std::vector<std::string>&	getJoinedChannel() const;
	void	clearJoinedChannel();
	void	addJoinedChannel(const std::string& channel_name);
	void	deleteJoinedChannel(const std::string& channel_name);
private :
	std::string _nickname;
	std::string	_username;
	std::string	_realname;
	std::string _hostname;
	std::string _servername;
	std::string	_command;
	int			_fd;
	bool		_pass;
	bool		_nick;
	bool		_user;
	bool		_allready;
	bool		_disconnect;
	std::vector<std::string>	_message;
	std::vector<std::string>	_joined_channel;
};