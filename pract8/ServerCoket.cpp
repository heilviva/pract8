#define WIN32_LEAN_AND_MEAN // макрос

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
using namespace std;

int main()
{
	// служебная структура для хранение информации (ВЕРСИИ, СТРУКТУРЫ(НАПРИМЕР СЕМЕЙНУЮ)
   // о реализации Windows Sockets
	WSADATA wsaData;
	SOCKET СonnectSocket = INVALID_SOCKET;
	SOCKET ListenSocket = INVALID_SOCKET;//слушает соединение
	ADDRINFO hints;
	ADDRINFO* addrResult;
	const char* sendBuffer = "Hello from Server";
	char recvBuffer[512];

	// старт использования библиотеки сокетов процессом определния версии и структуры
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	// Если произошла ошибка подгрузки библиотеки
	if (result != 0) {
		cout << "WSAStartup failed result" << endl;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));//нам необходимо изначально занулить память, 1-ый паметр, что зануляем,2-ой сколько
	hints.ai_family = AF_INET;//4-БАЙТНЫЙ Ethernet
	hints.ai_socktype = SOCK_STREAM; //задаем потоковый тип сокета
	hints.ai_protocol = IPPROTO_TCP;// Используем протокол TCP
	hints.ai_flags = AI_PASSIVE;// Пассивная сторона, потому что просто ждет соединения

	// функциия хранит в себе адрес, порт,семейство структур, адрес сокета
	result = getaddrinfo(NULL, "666", &hints, &addrResult);
	// Если инициализация структуры адреса завершилась с ошибкой,
	// выведем сообщением об этом и завершим выполнение программы 
	if (result != 0) {
		cout << "getaddrinfo failed with error" << endl;
		WSACleanup();//очистка WSAStartup
		return 1;
	}

	ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	// Если создание сокета завершилось с ошибкой, выводим сообщение,
   // освобождаем память, выделенную под структуру addr,
	if (ListenSocket == INVALID_SOCKET) {
		cout << "socket error" << endl;
		WSACleanup();//очситка WSAStartup
		freeaddrinfo(addrResult);
		return 1;
	}
	// Привязываем сокет к IP-адресу (соединились с сервером)
	result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	// Если привязать адрес к сокету не удалось, то выводим сообщение
   // об ошибке, освобождаем память, выделенную под структуру addr.
   // и закрываем открытый сокет.
   // Выгружаем DLL-библиотеку из памяти и закрываем программу.
	if (result == SOCKET_ERROR) {
		cout << "Binding connect failed <3" << endl;
		closesocket(СonnectSocket);
		ListenSocket = INVALID_SOCKET;
		freeaddrinfo(addrResult);
		WSACleanup();//очситка WSAStartup
		return 1;
	}
	result = listen(ListenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		cout << "Listening failed" << endl;
		closesocket(СonnectSocket);
		ListenSocket = INVALID_SOCKET;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	СonnectSocket = accept(ListenSocket, NULL, NULL);
	if (СonnectSocket == INVALID_SOCKET) {
		cout << "Accepting SOCKET failed" << endl;
		closesocket(СonnectSocket);
		ListenSocket = INVALID_SOCKET;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	closesocket(ListenSocket);

	do {
		ZeroMemory(recvBuffer, 512);
		result = recv(СonnectSocket, recvBuffer, 512, 0);
		if (result > 0) {
			cout << "Received " << result << " bytes" << endl;
			cout << "Received data: " << recvBuffer << endl;

			// Ответ клиенту
			result = send(СonnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
			if (result == SOCKET_ERROR) {
				cout << "Failed to send" << endl;
				WSACleanup();
				return 1;
			}
		}
		else if (result == 0) {
			cout << "Connection closing" << endl;
		}
		else {
			cout << "recv failed with error" << endl;
		}

		// Здесь сервер продолжит цикл, ожидая следующие данные от клиента
	} while (result > 0);
	result = shutdown(СonnectSocket, SD_SEND);
	if (result == SOCKET_ERROR) {
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	closesocket(СonnectSocket);
	freeaddrinfo(addrResult);
	WSACleanup();
	return 0;
}