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
		cout << "Что будем делать?" << endl
			<< "1. Регистрация нового пользователя." << endl
			<< "2. Авторизация нового пользователя." << endl
			<< "3. Пойду спать." << endl << endl << "Вы ввели: ";
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
		cout << "Что будем делать дальше?" << endl
			<< "1. Посмотреть кто сейчас в сети." << endl
			<< "2. Отправить сообщение пользователю в сети." << endl
			<< "3. Отправить сообщение всем пользователям в сети." << endl
			<< "4. Посмотреть от кого есть сообщения." << endl
			<< "5. Посмотреть сообщения от пользователя." << endl
			<< "6. Теперь точно спать." << endl << endl << "Вы ввели: ";
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