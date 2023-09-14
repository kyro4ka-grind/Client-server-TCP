#include "Client.h"

int main()
{
	setlocale(LC_ALL, "RUS");

	Client client;
	thread listenThread([&client]() { client.Listening(); });	//Thread to listen for answer from server
	listenThread.detach();

	int request;
	while (true)
	{
		cout << "��� ����� ������?" << endl
			<< "1. ����������� ������ ������������." << endl
			<< "2. ����������� ������ ������������." << endl
			<< "3. ����� �����." << endl << endl << "�� �����: ";
		cin >> request;
		cout << endl;
		switch (request)
		{
		case 1:
			client.Registration();
			continue;
		case 2:
			if (!client.Authorization())
			{
				cout << endl << endl;
				continue;
			}
			break;
		default:
			exit(0);
		}

		break;
	}

	while (true)
	{
		cout << "��� ����� ������ ������?" << endl
			<< "1. ���������� ��� ������ � ����." << endl
			<< "2. ��������� ��������� ������������ � ����." << endl
			<< "3. ��������� ��������� ���� ������������� � ����." << endl
			<< "4. ���������� �� ���� ���� ���������." << endl
			<< "5. ���������� ��������� �� ������������." << endl
			<< "6. ������ ����� �����." << endl << endl << "�� �����: ";
		cin >> request;
		cout << endl;

		switch (request)
		{
		case 1:
			client.Online();
			continue;
		case 2:
			client.SendMsgToUser();
			continue;
		case 3:
			client.SendMsgToAllUsers();
			continue;
		case 4:
			client.NewMessages();
			continue;
		case 5:
			client.ReadMsgFromUser();
			continue;
		default:
			client.EndClient();
			client.~Client();
			exit(0);
		}
	}
}