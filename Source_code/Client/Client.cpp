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

#define SIGNUP "SIGNUP"
#define LOGIN  "LOGIN"
#define LOGOUT "LOGOUT"
#define START "START"
#define QUESTION "QUESTION"
#define ANSWER "ANSWER"
#define RESULT "RESULT"
#define ASSIST "ASSIST"
#define QUIT "QUIT"
#define VICTORY "VICTORY"
#define SERVER_ERROR "10"
#define BAD_REQUEST "20"
#define CANNOT_USE_ASSIST "21"
#define CANNOT_START_GAME "22"
#define CANNOT_ANSWER "23"
#define CANNOT_QUIT "24"
#define SUCCESS "30"

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

string request, response;

string recv(SOCKET &currSoc);
void send(SOCKET &currSoc, char req[]);
void menu();
void showHD();
void startGame();
void playGame();
void showQues();
string signUp();
string logIn();
string logOut();
string start();
string answer();
string assist();
string quit();
void solveRequest();
void solveResponse(char *);
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
	int ret, messageLen;
	while (1) {
		menu();
		messageLen = request.size();
		if (messageLen == 0) {
			throw MSG_EMPTY;
			break;
		}
		send(client, &request[0]);
		request = "";
		// receive message
		response = recv(client);
		response[response.size() - 2] = 0;
		solveResponse(&response[0]);
		request = "";
		response = "";
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
	int select;
	do
	{
		cout << "\nChon muc muon chon: ";
		cin >> select;
		if (select == 1)
			system("cls");
		if (select < 1 || select>3)
			cout << " Chon khong hop le vui long chon lai \n";
	} while (select<1 || select>3);
	system("cls");
	if (select == 1) startGame();
	else if (select == 2) {
		showHD();
		int t;
		cin >> t;
		if (t == 1)
		{
			Sleep(200);
			system("cls");
			menu();
		}
		else exit(0);
	}
	else if (select == 3) exit(0);
}

void startGame() {
	VeKhung baoNgoai = VeKhung(35, 5, 50, 25, "");
	VeKhung batDau = VeKhung(46, 8, 27, 5, "        1. Dang ky");
	VeKhung huongDan = VeKhung(46, 15, 27, 5, "       2. Dang nhap");
	VeKhung thoat = VeKhung(46, 22, 27, 5, "        3. Quay lai");
	system("cls");
	baoNgoai.Ve();
	batDau.Ve();
	huongDan.Ve();
	thoat.Ve();
	cout << "\r\n\r\n\r\n\r\n\r\n\r\n\r\n";
	int select;
	do
	{
		cout << "\nChon muc muon chon: ";
		cin >> select;
		if (select == 1)
			system("cls");
		if (select < 1 || select>3)
			cout << " Chon khong hop le vui long chon lai \n";
	} while (select<1 || select>3);
	system("cls");
	if (select == 1) request = signUp();
	else if (select == 2) {
		request = logIn();
	}
	else if (select == 3) {
		Sleep(200);
		menu();
	}
}

void playGame() {
	VeKhung baoNgoai = VeKhung(28, 5, 50, 25, "");
	VeKhung batDau = VeKhung(46, 8, 27, 5, "        1. Choi ngay");
	VeKhung thoat = VeKhung(46, 15, 27, 5, "        2. Quay lai");
	system("cls");
	baoNgoai.Ve();
	batDau.Ve();
	thoat.Ve();
	cout << "\r\n\r\n\r\n\r\n\r\n\r\n\r\n";
	int select;
	do
	{
		cout << "\nChon muc muon chon: ";
		cin >> select;
		if (select == 1)
			system("cls");
		if (select < 1 || select>3)
			cout << " Chon khong hop le vui long chon lai \n";
	} while (select<1 || select>2);
	system("cls");
	if (select == 1) request = start();
	else if (select == 2) {
		Sleep(200);
		startGame();
	}
}
void showQues() {

}
void showHD() {
	cout << "1.  You can play as an entire class or in two groups.\n";
	cout << "If you play as an entire class, it is suggested that\n";
	cout << "you call on different students to answer questions.\n";
	cout << "If you play as two teams, you will need to play two games.\n";
	cout << "You will need to appoint a spokesperson for each team.\n";
	cout << "I have also played with two teams, alternating questions\n";
	cout << "between the two and keeping score.\n";
	cout << "2.  Once the game has loaded, and is in slide show view,\n";
	cout << "click on the $100 question.This game is very easy\n";
	cout << "to use since many of the directions are on each slide.\n";
	cout << "If a student would like to use 50 / 50, phone a friend\n";
	cout << "or ask the audience, click on the appropriate button.\n";
	cout << "Then follow the slides.\n";
	cout << "3. Directions for viewing the previous slide are given\n";
	cout << "on each slide.\n";
	cout << "4.  The game is over when a question is missed.1.You can play as an entire class or in two groups.\n";
	cout << "If you play as an entire class, it is suggested that\n";
	cout << "you call on different students to answer questions.\n";
	cout << "If you play as two teams, you will need to play two games.\n";
	cout << "You will need to appoint a spokesperson for each team.\n";
	cout << "I have also played with two teams, alternating questions\n";
	cout << "between the two and keeping score.\n";
	cout << "2.  Once the game has loaded, and is in slide show view,\n";
	cout << "click on the $100 question.This game is very easy\n";
	cout << "to use since many of the directions are on each slide.\n";
	cout << "If a student would like to use 50 / 50, phone a friend\n";
	cout << "or ask the audience, click on the appropriate button.\n";
	cout << "Then follow the slides.\n";
	cout << "3. Directions for viewing the previous slide are given\n";
	cout << "on each slide.\n";
	cout << "4.  The game is over when a question is missed.\n";
	cout << "\n Enter 1 to back menu";
	cout << "\n Enter anything to quit program\n";
}
string signUp() {
	cin >> request;
	// append request
	return request;
}
string logIn() {
	cin >> request;
	// append request
	return request;
}
string logOut() {
	// append request
	return request;
}

string start() {
	// append request
	return request;
}
string answer() {
	cin >> request;
	// append request
	return request;
}
string assist() {
	cin >> request;
	// append request
	return request;
}
string quit() {
	// append request
	return request;
}
void solveRequest() {

}
void solveResponse(char *str) {
	//show message from server
}
