#define WIN32_LEAN_AND_MEAN // ������

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
using namespace std;

int main()
{
	// ��������� ��������� ��� �������� ���������� (������, ���������(�������� ��������)
   // � ���������� Windows Sockets
	WSADATA wsaData;
	SOCKET �onnectSocket = INVALID_SOCKET;
	SOCKET ListenSocket = INVALID_SOCKET;//������� ����������
	ADDRINFO hints;
	ADDRINFO* addrResult;
	const char* sendBuffer = "Hello from Server";
	char recvBuffer[512];

	// ����� ������������� ���������� ������� ��������� ���������� ������ � ���������
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	// ���� ��������� ������ ��������� ����������
	if (result != 0) {
		cout << "WSAStartup failed result" << endl;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));//��� ���������� ���������� �������� ������, 1-�� ������, ��� ��������,2-�� �������
	hints.ai_family = AF_INET;//4-������� Ethernet
	hints.ai_socktype = SOCK_STREAM; //������ ��������� ��� ������
	hints.ai_protocol = IPPROTO_TCP;// ���������� �������� TCP
	hints.ai_flags = AI_PASSIVE;// ��������� �������, ������ ��� ������ ���� ����������

	// �������� ������ � ���� �����, ����,��������� ��������, ����� ������
	result = getaddrinfo(NULL, "666", &hints, &addrResult);
	// ���� ������������� ��������� ������ ����������� � �������,
	// ������� ���������� �� ���� � �������� ���������� ��������� 
	if (result != 0) {
		cout << "getaddrinfo failed with error" << endl;
		WSACleanup();//������� WSAStartup
		return 1;
	}

	ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	// ���� �������� ������ ����������� � �������, ������� ���������,
   // ����������� ������, ���������� ��� ��������� addr,
	if (ListenSocket == INVALID_SOCKET) {
		cout << "socket error" << endl;
		WSACleanup();//������� WSAStartup
		freeaddrinfo(addrResult);
		return 1;
	}
	// ����������� ����� � IP-������ (����������� � ��������)
	result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	// ���� ��������� ����� � ������ �� �������, �� ������� ���������
   // �� ������, ����������� ������, ���������� ��� ��������� addr.
   // � ��������� �������� �����.
   // ��������� DLL-���������� �� ������ � ��������� ���������.
	if (result == SOCKET_ERROR) {
		cout << "Binding connect failed <3" << endl;
		closesocket(�onnectSocket);
		ListenSocket = INVALID_SOCKET;
		freeaddrinfo(addrResult);
		WSACleanup();//������� WSAStartup
		return 1;
	}
	result = listen(ListenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		cout << "Listening failed" << endl;
		closesocket(�onnectSocket);
		ListenSocket = INVALID_SOCKET;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	�onnectSocket = accept(ListenSocket, NULL, NULL);
	if (�onnectSocket == INVALID_SOCKET) {
		cout << "Accepting SOCKET failed" << endl;
		closesocket(�onnectSocket);
		ListenSocket = INVALID_SOCKET;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	closesocket(ListenSocket);

	do {
		ZeroMemory(recvBuffer, 512);
		result = recv(�onnectSocket, recvBuffer, 512, 0);
		if (result > 0) {
			cout << "Received " << result << " bytes" << endl;
			cout << "Received data: " << recvBuffer << endl;

			// ����� �������
			result = send(�onnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
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

		// ����� ������ ��������� ����, ������ ��������� ������ �� �������
	} while (result > 0);
	result = shutdown(�onnectSocket, SD_SEND);
	if (result == SOCKET_ERROR) {
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	closesocket(�onnectSocket);
	freeaddrinfo(addrResult);
	WSACleanup();
	return 0;
}