#include "Server.h"
#pragma warning(suppress : 4996)

Server::Server()
{
	_port = 12345;		//Connection port
	StartServer();
}

Server::~Server()
{
	EndServer();
}

void Server::StartServer()
{
	if (WSAStartup(MAKEWORD(2, 2), &_wData) != 0)
	{
		cout << "ERROR: WsaStartup failed: " << WSAGetLastError() << endl;
		return;
	}

	_listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//SOCK_STREAM - TCP
	if (_listeningSocket == INVALID_SOCKET)				
	{
		cout << "ERROR: Invalid socket: " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	_serverAddr.sin_family = AF_INET;							//Using IpV4
	_serverAddr.sin_port = htons(_port);
	_serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");;		//Server enable on any local machine

	if (bind(_listeningSocket, (SOCKADDR*)&_serverAddr, sizeof(_serverAddr)) == SOCKET_ERROR)	//Bind listening socket to address
	{
		cout << "ERROR: Bind failed: " << WSAGetLastError() << endl;
		closesocket(_listeningSocket);
		WSACleanup();
		return;
	}

	listen(_listeningSocket, 100);					//Listen socket, max count of users is 100
	cout << "Listening . . ." << endl;
}

void Server::EndServer()
{
	closesocket(_listeningSocket);
	WSACleanup();
	cout << "The server has been stoped." << endl;
}

void Server::Accepting()
{
	int sizeClientAddr = sizeof(_clientAddr);

	while (true)
	{
		_clientSocket = accept(_listeningSocket, (SOCKADDR*)&_clientAddr, &sizeClientAddr);	//Accept connection
		cout << "Пользователь подключился к серверу." << endl;
		thread listenThread([this]() { Listening(_clientAddr, _clientSocket); });
		listenThread.detach();
	}
}

void Server::Listening(SOCKADDR_IN addr, SOCKET sock)
{
	string request, msg;

	while (true)
	{
		msg = GetMsg(addr, sock);					//Get message from client
		Sleep(50);
		if (msg == "")
			continue;

		while (true)						//Separating request from the message
		{
			if (msg[0] == '|')
			{
				msg.erase(0, 1);
				break;
			}
			request.push_back(msg[0]);
			msg.erase(0, 1);
		}

		if (request == "0")
		{
			Disconect(addr, sock, msg);
			cout << "User disconected." << endl;
			return;
		}

		if (request == "1")
		{
			Registration(msg, addr, sock);
			request = "";
		}

		if (request == "2")
		{
			Authorization(msg, addr, sock);
			request = "";
		}

		if (request == "3")
		{
			Online(addr, sock);
			request = "";
		}

		if (request == "4")
		{
			OnlineUser(addr, sock, msg);
			request = "";
		}

		if (request == "5")
		{
			SendMsgToUser(addr, sock, msg);
			request = "";
		}

		if (request == "6")
		{
			SendMsgToAllUsers(addr, sock, msg);
			request = "";
		}

		if (request == "7")
		{
			NewMessages(addr, sock, msg);
			request = "";
		}

		if (request == "8")
		{
			ReadMsgFromUser(addr, sock, msg);
			request = "";
		}
	}
}

string Server::GetMsg(SOCKADDR_IN addr, SOCKET sock)
{
	char buffMsg[1024] = {0};
	int sizeMsg = 0;
	int sizeAddr = sizeof(addr);

	sizeMsg = recvfrom(sock, &buffMsg[0], sizeof(buffMsg), 0, (sockaddr*)&addr, &sizeAddr);
	if (sizeMsg != -1)
	{
		cout << "Message received: " << buffMsg << endl;
		return buffMsg;
	}
	return "";
}

void Server::SendMsg(string msg, SOCKADDR_IN addr, SOCKET sock)
{
	int sizemsg = msg.length();
	int sizeAddr = sizeof(addr);

	sendto(sock, msg.c_str(), sizemsg, 0, (sockaddr*)&addr, sizeAddr);
	std::cout << "Message: " << msg << " has been send." << endl;
}

void Server::Registration(string msg, SOCKADDR_IN addr, SOCKET sock)
{
	string nickname, password;
	string nicknameBuff, passwordBuff;
	int size = 0;

	while (true)						//Separating nickname from the message
	{
		if (msg[0] == '|')
		{
			msg.erase(0, 1);
			break;
		}
		nickname.push_back(msg[0]);
		msg.erase(0, 1);
	}
	password = msg;

	_login.open("Clients.bin", ios::in | ios::binary);

	_login.read((char*)&size, sizeof(int));				//Read size
	nicknameBuff.resize(size, '\0');					//Resize for size
	_login.read((char*)nicknameBuff.c_str(), size);		//Read information from file

	_login.read((char*)&size, sizeof(int));
	passwordBuff.resize(size, '\0');
	_login.read((char*)passwordBuff.c_str(), size);

	if (nickname == nicknameBuff)
	{
		SendMsg("0", addr, sock);
		return;
	}
	_login.close();

	//Writing a new user to a file
	_login.open("Clients.bin", ios::app | ios::binary);

	size = nickname.length();
	_login.write((char*)&size, sizeof(int));
	_login.write(nickname.c_str(), size);

	size = password.length();
	_login.write((char*)&size, sizeof(int));
	_login.write(password.c_str(), size);

	_login.close();
	SendMsg("1", addr, sock);
}

void Server::Authorization(string msg, SOCKADDR_IN addr, SOCKET sock)
{
	string nickname, password;
	string nicknameBuff, passwordBuff;
	int size = 0;

	while (true)						//Separating nickname from the message
	{
		if (msg[0] == '|')
		{
			msg.erase(0, 1);
			break;
		}
		nickname.push_back(msg[0]);
		msg.erase(0, 1);
	}
	password = msg;

	_login.open("Clients.bin", ios::in | ios::binary);

	while (!_login.eof() && _login.peek() != EOF)
	{
		_login.read((char*)&size, sizeof(int));				//Read size
		nicknameBuff.resize(size, '\0');					//Resize for size
		_login.read((char*)nicknameBuff.c_str(), size);		//Read information from file

		_login.read((char*)&size, sizeof(int));
		passwordBuff.resize(size, '\0');
		_login.read((char*)passwordBuff.c_str(), size);

		if (nickname == nicknameBuff && password == passwordBuff)
		{
			//Checking that there is no such user on the server
			for (auto &el : _clients)
			{
				if (el.nickname == nicknameBuff)
				{
					SendMsg("-1", addr, sock);
					return;
				}
			}

			SendMsg("1", addr, sock);
			cout << "Пользователь под ником: " << nicknameBuff
				<< " подключился к серверу." << endl;

			//Пользователь становится онлайн
			_clients.push_back(Client(nicknameBuff));

			_login.close();
			return;
		}
	}

	_login.close();
	SendMsg("0", addr, sock);
}

void Server::Online(SOCKADDR_IN addr, SOCKET sock)
{
	string sendingBuff;
	for (auto &client : _clients)
	{
		sendingBuff += client.nickname + '|';
	}

	SendMsg(sendingBuff, addr, sock);
}

void Server::OnlineUser(SOCKADDR_IN addr, SOCKET sock, string msg)
{
	for (auto &client : _clients)
	{
		if (client.nickname == msg)
		{
			SendMsg("1", addr, sock);
			return;
		}
	}
	SendMsg("0", addr, sock);
}

void Server::SendMsgToUser(SOCKADDR_IN addr, SOCKET sock, string msg)
{
	string nickname, nicknameFrom;

	while (true)						//Separating nickname from the message
	{
		if (msg[0] == '|')
		{
			msg.erase(0, 1);
			break;
		}
		nickname.push_back(msg[0]);
		msg.erase(0, 1);
	}

	while (true)						//Separating nicknameFrom from the message
	{
		if (msg[0] == '|')
		{
			msg.erase(0, 1);
			break;
		}
		nicknameFrom.push_back(msg[0]);
		msg.erase(0, 1);
	}

	for (auto &client : _clients)
	{
		if (client.nickname == nickname)
		{
			for (auto &messagesFrom : client.messagesFrom)
			{
				if (messagesFrom.nickfrom == nicknameFrom)
				{
					messagesFrom.messages.push_back(msg);
					return;
				}
			}
			client.messagesFrom.push_back(MessageFrom(msg, nicknameFrom));
			return;
		}
	}
}

void Server::SendMsgToAllUsers(SOCKADDR_IN addr, SOCKET sock, string msg)
{
	string nicknameFrom;
	int flag = 0;

	while (true)						//Separating nicknameFrom from the message
	{
		if (msg[0] == '|')
		{
			msg.erase(0, 1);
			break;
		}
		nicknameFrom.push_back(msg[0]);
		msg.erase(0, 1);
	}

	for (auto &client : _clients)
	{
		for (auto &messagesFrom : client.messagesFrom)
		{
			if (messagesFrom.nickfrom == nicknameFrom)
			{
				messagesFrom.messages.push_back(msg);
				flag = 1;
				break;
			}
		}
		if (flag == 0)
		{
			client.messagesFrom.push_back(MessageFrom(msg, nicknameFrom));
			continue;
		}
		flag = 0;
	}
}

void Server::NewMessages(SOCKADDR_IN addr, SOCKET sock, string nickname)
{
	string buff;

	for (auto &client : _clients)
	{
		if (client.nickname == nickname)
		{
			for (auto &nicknameFrom : client.messagesFrom)
			{
				buff += nicknameFrom.nickfrom + '|';
			}
			break;
		}
	}

	if (buff == "")
	{
		SendMsg("|", addr, sock);
		return;
	}
	SendMsg(buff, addr, sock);
}

void Server::ReadMsgFromUser(SOCKADDR_IN addr, SOCKET sock, string nicknameFrom)
{
	string nickname;
	string buff;

	while (true)						//Separating nickname from the message
	{
		if (nicknameFrom[0] == '|')
		{
			nicknameFrom.erase(0, 1);
			break;
		}
		nickname.push_back(nicknameFrom[0]);
		nicknameFrom.erase(0, 1);
	}
	cout << nickname << endl;

	for (auto &client : _clients)
	{
		if (client.nickname == nickname)
		{
			for (auto& messageFrom : client.messagesFrom)
			{
				if (messageFrom.nickfrom == nicknameFrom)
				{
					for (auto& message : messageFrom.messages)
					{
						buff += message + '|';
					}
					SendMsg(buff, addr, sock);
					cout << buff;
					return;
				}
			}
			SendMsg("0", addr, sock);
			return;
		}
	}
}

void Server::Disconect(SOCKADDR_IN addr, SOCKET sock, string nickname)
{
	int position = 0;

	for (auto &client : _clients)
	{
		if (client.nickname == nickname)
		{
			break;
		}
		position++;
	}

	_clients.erase(_clients.begin() + position);
}