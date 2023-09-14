#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <locale.h>
#include <vector>
#include <thread>
#include <string>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
using namespace std;

class Client
{
public:
	Client();
	~Client();
	void StartClient();
	void EndClient();
	void Listening();
	void SendMsg(string msg);
	string GetMsg();

	void Registration();
	bool Authorization();
	void Online();
	void SendMsgToUser();
	bool OnlineUser(string nickname);
	void SendMsgToAllUsers();
	void NewMessages();
	void ReadMsgFromUser();

private:
	SOCKET		_clientSock;
	SOCKADDR_IN	_addr;
	WSAData		_wData;
	int			_port;
	string		_buffAnswer;
	string		_nickname;
};