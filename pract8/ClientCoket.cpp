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

	SOCKET ConnectSocket = INVALID_SOCKET;

	ADDRINFO hints;
	ADDRINFO* addrResult;

	const char* sendBuffer = "hello from Client";
	char recvBuffer[512];

	// Старт использования библиотеки сокетов: определение версии и структуры
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	// Если произошла ошибка подгрузки библиотеки
	if (result != 0)
	{
		cout << "WSAStartup failed result" << endl;
		WSACleanup();
		return 1;
	}

	// Инициализация hints
	ZeroMemory(&hints, sizeof(hints));//нам необходимо изначально занулить память, 1-ый паметр, что зануляем,2-ой сколько
	hints.ai_family = AF_INET;//4-БАЙТНЫЙ Ethernet
	hints.ai_socktype = SOCK_STREAM; //задаем потоковый тип сокета
	hints.ai_protocol = IPPROTO_TCP;// Используем протокол TCP

	// функциия хранит в себе адрес, порт,семейство структур, адрес сокета
	result = getaddrinfo(NULL, "666", &hints, &addrResult);
	// Если инициализация структуры адреса завершилась с ошибкой,
	// выведем сообщением об этом и завершим выполнение программы 
	if (result != 0) {
		cout << "getaddrinfo failed with error" << endl;
		WSACleanup();//очситка WSAStartup
		return 1;
	}
	

	// Создание клиентского сокета
	ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET)
	{
		cout << "getaddrinfo creation error" << endl;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	// Подключение к серверу
	result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if (result == SOCKET_ERROR)
	{
		cout << " connect failed" << endl;
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	// Отправка данных 
	result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
	if (result == SOCKET_ERROR)
	{
		cout << "send failed error" << result << endl;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	cout << "sent" << result << "bytes" << endl;
	if (result == SOCKET_ERROR)
	{
		result = shutdown(ConnectSocket, SD_SEND);
		cout << "shutdown error" << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
	if (result == SOCKET_ERROR)
	{
		cout << "send failed error" << result << endl;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	
	result = recv(ConnectSocket, recvBuffer, sizeof(recvBuffer), 0);
	if (result > 0) {
		recvBuffer[result] = '\0';
		cout << "Received " << result << " bytes: " << recvBuffer << endl;
	}
	else if (result == 0) {
		cout << "Connection closed by server" << endl;
	}
	else {
		cout << "recv failed with error" << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}


	// конец сеанса
	freeaddrinfo(addrResult);
	WSACleanup();
	return 1;
}