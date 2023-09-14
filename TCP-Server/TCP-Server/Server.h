#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <locale.h>
#include <vector>
#include <thread>
#include <mutex>
#include "fstream"
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
using namespace std;

struct MessageFrom
{
	MessageFrom(string message, string nick)
	{
		messages.push_back(message);
		nickfrom = nick;
	}
	vector<string> messages;
	string nickfrom;
};

struct Client
{
	Client(string _nickname)
	{
		nickname = _nickname;
	}
	string nickname;
	vector<MessageFrom> messagesFrom;
};

class Server
{
public:
	Server();
	~Server();
	void Accepting();	//Accepting clients
	void Listening(SOCKADDR_IN addr, SOCKET sock);	//Listening requests

	string GetMsg(SOCKADDR_IN addr, SOCKET sock);
	void SendMsg(string msg, SOCKADDR_IN addr, SOCKET sock);
	void Registration(string msg, SOCKADDR_IN addr, SOCKET sock);
	void Authorization(string msg, SOCKADDR_IN addr, SOCKET sock);
	void Online(SOCKADDR_IN addr, SOCKET sock);
	void OnlineUser(SOCKADDR_IN addr, SOCKET sock, string msg);
	void SendMsgToUser(SOCKADDR_IN addr, SOCKET sock, string msg);
	void SendMsgToAllUsers(SOCKADDR_IN addr, SOCKET sock, string msg);
	void NewMessages(SOCKADDR_IN addr, SOCKET sock, string nickname);
	void ReadMsgFromUser(SOCKADDR_IN addr, SOCKET sock, string nicknameFrom);
	void Disconect(SOCKADDR_IN addr, SOCKET sock, string nickname);

private:
	void StartServer();	//Start server
	void EndServer();	//Close server

	SOCKET			_listeningSocket;
	SOCKADDR_IN		_serverAddr;
	SOCKET			_clientSocket;
	SOCKADDR_IN		_clientAddr;
	WSAData			_wData;
	unsigned short	_port;
	vector<Client>	_clients;	//Online users
	fstream			_login;
};