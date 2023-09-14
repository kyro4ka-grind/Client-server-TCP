#include "Client.h"

Client::Client()
{
	_port = 12345;
	StartClient();
}

Client::~Client()
{
	closesocket(_clientSock);
	WSACleanup();
	cout << "Bye bye!" << endl;
}

void Client::StartClient()
{
	if (WSAStartup(MAKEWORD(2, 2), &_wData) != 0)
	{
		cout << "ERROR: WsaStartup failed." << endl;
		exit(0);
	}

	_clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//SOCK_STREAM - TCP
	if (_clientSock == INVALID_SOCKET)
	{
		cout << "ERROR: Invalid socket." << endl;
		WSACleanup();
		exit(0);
	}

	_addr.sin_family = AF_INET;							//Using IpV4
	_addr.sin_port = htons(_port);
	_addr.sin_addr.s_addr = inet_addr("127.0.0.1");		//Server enable on any local machine

	if (connect(_clientSock, (sockaddr*)&_addr, sizeof(_addr)))
	{
		cout << "ERROR: Connection error." << endl;
		exit(0);
	}
}

void Client::EndClient()
{
	SendMsg("0|" + _nickname);
}

string Client::GetMsg()
{
	int ClientAddrSize = sizeof(_clientSock);
	char msg[1024] = { 0 };

	recvfrom(_clientSock, msg, 1024, 0, (sockaddr*)&_addr, &ClientAddrSize);	//Receive message

	return msg;
}

void Client::SendMsg(string msg)
{
	int sizemsg = msg.length();		//Determine size of the string to send

	sendto(_clientSock, msg.c_str(), sizemsg, 0, (sockaddr*) & _addr, sizeof(_addr));
}

void Client::Listening()
{
	while (true)
	{
		_buffAnswer = GetMsg();
		Sleep(50);
	}
}

void Client::Registration()
{
	string request, nickname, password;

	cout << "������� ���� ������� �������: ";
	cin >> nickname;
	cout << endl;

	cout << "������� ������: ";
	cin >> password;
	cout << endl;

	request += "1|" + nickname + '|' + password;
	SendMsg(request);

	Sleep(100);
	if (_buffAnswer == "0")
	{
		cout << "������������ � ����� ��������� ��� ���������������." << endl << endl;
		_buffAnswer = "";
		return;
	}
	else if (_buffAnswer == "1")
	{
		cout << "����������� ������ �������." << endl << endl;
		_buffAnswer = "";
		return;
	}
	else
	{
		cout << "������ �� ��� ������(" << endl << endl;
		return;
	}
}

bool Client::Authorization()
{
	string request, nickname, password;

	cout << "������� ���� �������: ";
	cin >> nickname;
	cout << endl;

	cout << "������� ���� ������: ";
	cin >> password;
	cout << endl;

	request += "2|" + nickname + '|' + password;
	SendMsg(request);

	Sleep(100);
	if (_buffAnswer == "0")
	{
		cout << "������� ������������ ������." << endl << endl;
		_buffAnswer = "";
		return false;
	}
	else if (_buffAnswer == "1")
	{
		cout << "����������� ������ �������." << endl
			<< "����� ���������� " << nickname << "!" << endl << endl;
		_buffAnswer = "";
		_nickname = nickname;
		return true;
	}
	else if (_buffAnswer == "-1")
	{
		cout << "����� ������������ ��� � ����." << endl << endl;
		_buffAnswer = "";
		return false;
	}
	else
	{
		cout << "������ �� ��� ������(" << endl << endl;
		return false;
	}
}

void Client::Online()
{
	string buff;
	int count = 0;
	SendMsg("3|");
	Sleep(100);

	while (_buffAnswer != "")
	{
		while (true)						//Separating nickname from the message
		{
			if (_buffAnswer[0] == '|')
			{
				_buffAnswer.erase(0, 1);
				count++;
				break;
			}
			buff.push_back(_buffAnswer[0]);
			_buffAnswer.erase(0, 1);
		}
		cout << count << ". " << buff << endl;
		buff = "";
	}
	cout << endl << endl;
}

bool Client::OnlineUser(string nickname)
{
	SendMsg("4|" + nickname);
	Sleep(100);

	if (_buffAnswer == "0")
	{
		_buffAnswer = "";
		return false;
	}
	else if (_buffAnswer == "1")
	{
		_buffAnswer = "";
		return true;
	}
	else
	{
		cout << "������ �� ��� ������(" << endl;
		return false;
	}
}

void Client::SendMsgToUser()
{
	string msg, nickname;

	while (true)
	{
		cout << "������� ��� ������������, ���� ������ �� ��������� ��������: ";
		cin >> nickname;

		if (OnlineUser(nickname))
			break;
		cout << "������������ � ����� ����� ������ �� � ����(" << endl;
		return;
	}

	cout << endl << "������� ���������: ";
	getchar();
	getline(cin, msg);
	cout << endl;

	SendMsg("5|" + nickname + '|' + _nickname + '|' + msg);
	cout << "��������� ����������." << endl;
}

void Client::SendMsgToAllUsers()
{
	string msg;

	cout << "������� ���������, ������� ������� ��� ������������ � ����: ";
	getchar();
	getline(cin, msg);
	cout << endl;

	SendMsg("6|" + _nickname + '|' + msg);
	cout << "��������� ����������." << endl;
}

void Client::NewMessages()
{
	string buff;
	int count = 0;

	SendMsg("7|" + _nickname);
	Sleep(100);

	cout << "� ��� ��������� ��: " << endl;
	if (_buffAnswer == "|")
	{
		_buffAnswer = "";
		cout << "����� ��������� ���(" << endl << endl;
		return;
	}
	else if (_buffAnswer == "")
	{
		cout << "������ �� ��������(" << endl << endl;
	}
	while (_buffAnswer != "")
	{
		while (true)						//Separating nicknames from the message
		{
			if (_buffAnswer[0] == '|')
			{
				_buffAnswer.erase(0, 1);
				count++;
				break;
			}
			buff.push_back(_buffAnswer[0]);
			_buffAnswer.erase(0, 1);
		}
		cout << count << ". " << buff << endl;
		buff = "";
	}
	cout << endl << endl;
}

void Client::ReadMsgFromUser()
{
	string nickname, buff;

	cout << "������� ������� ������������, ��������� �������� ������ ���������: ";
	cin >> nickname;

	SendMsg("8|" + _nickname + '|' + nickname);
	Sleep(100);

	if (_buffAnswer == "0")
	{
		cout << "��������� �� ����� ������������ ���(" << endl;
		return;
	}
	else if (_buffAnswer == "")
	{
		cout << "������ �� ��������(" << endl << endl;
	}

	cout << "��������� �� ������������ " << nickname << ":" << endl;
	while (_buffAnswer != "")
	{
		while (true)						//Separating messages from the message
		{
			if (_buffAnswer[0] == '|')
			{
				_buffAnswer.erase(0, 1);
				break;
			}
			buff.push_back(_buffAnswer[0]);
			_buffAnswer.erase(0, 1);
		}
		cout << buff << endl;
		buff = "";
	}
	cout << endl << endl;
}