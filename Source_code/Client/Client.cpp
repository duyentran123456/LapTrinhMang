// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "VeKhung.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <winsock2.h>
#include <WS2tcpip.h>
#define BUFF_SIZE  2048
#define MSG_EMPTY 0

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

string recv(SOCKET &currSoc);
void send(SOCKET &currSoc, char req[]);
void game();
void menu();
void sign_up();
void log_in();
void start_game();
void answer();
void assist();
void quit();
void solveRequest();
void solveResponse();
int main(int argc, char* argv[])
{
	// Step 1: Inittiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Winsock 2.2 is not supported.\n");
		return 0;
	}

	// Step 2: Constuct socket
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client == INVALID_SOCKET) {
		printf("Error %d: cannot create server socket.", WSAGetLastError());
		return 0;
	}

	//Step 3: Specify server address
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(stoi(argv[2]));
	inet_pton(AF_INET, argv[1], &serverAddr.sin_addr);

	//Step 4: Request to connect server
	if (connect(client, (sockaddr *)&serverAddr, sizeof(serverAddr))) {
		printf("Error %d: Cannot connect server.", WSAGetLastError());
		return 0;
	}
	printf("Connected server!\n");

	//Step 5: Communicate with server
	string buff;
	string res;
	int ret, messageLen;
	menu();
	system("cls");
	while (1) {

	}
	//Step 6: Close socket
	closesocket(client);
	//Step 7: Terminate Winsock
	WSACleanup();
	return 0;
}

string recv(SOCKET &currSoc) {
	char buff[BUFF_SIZE];
	string data;
	int ret, len = 0;
	while (1) {
		ret = recv(currSoc, buff, BUFF_SIZE, 0);
		if (ret == SOCKET_ERROR) {
			printf("Cannot receive message.");
			break;
		}
		buff[ret] = 0;
		len += ret;
		// append
		data.append(buff);
		bool check = false;
		for (int i = 0; i < len - 1; i++) {
			if (data[i] == '\r'&& data[i + 1] == '\n') check = true;
		}
		if (check) break;
	}
	return data;
}

void send(SOCKET &currSoc, char req[]) {
	int rLeft = strlen(req);
	int index = 0;
	int ret;
	while (rLeft > 0) {
		ret = send(currSoc, req + index, rLeft, 0);
		if (ret == SOCKET_ERROR) {
			printf("Cannot send data\n");
			return;
		}
		rLeft -= ret;
		index += ret;
	}
}

void menu() {
	VeKhung baoNgoai = VeKhung(35, 5, 50, 25, "");
	VeKhung batDau = VeKhung(46, 8, 27, 5, "        1. Bat Dau");
	VeKhung huongDan = VeKhung(46, 15, 27, 5, "       2. Huong Dan");
	VeKhung thoat = VeKhung(46, 22, 27, 5, "        3. Thoat");
	system("cls");
	baoNgoai.Ve();
	batDau.Ve();
	huongDan.Ve();
	thoat.Ve();
	cout << "\r\n\r\n\r\n\r\n\r\n\r\n\r\n";
	int chon;
	do
	{
		cout << "\nChon muc muon chon: ";
		cin >> chon;
		if (chon == 1)
			system("cls");
		if (chon < 1 || chon>3)
			cout << " Chon khong hop le vui long chon lai \n";
	} while (chon<1 || chon>3);
}