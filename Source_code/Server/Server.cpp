// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "gamesolution.h"
#include "utils.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")

#define PORT 6000
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 2048
using namespace std;
// struct
// function prototype
string recv(SOCKET &currSoc);
void send(SOCKET &currSoc, char req[]);
void solveRequest();
void sign_up();
void log_in();
void start_game();// solve request START of client
void answer();
void assist();
void quit();

int main(int argc, char* argv[])
{
	DWORD		nEvents = 0;
	DWORD		index;
	SOCKET		socks[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT	events[WSA_MAXIMUM_WAIT_EVENTS];
	WSANETWORKEVENTS sockEvent;

	//Step 1: Initiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Winsock 2.2 is not supported\n");
		return 0;
	}

	//Step 2: Construct LISTEN socket	
	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//Step 3: Bind address to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);

	socks[0] = listenSock;
	events[0] = WSACreateEvent(); //create new events
	nEvents++;

	// Associate event types FD_ACCEPT and FD_CLOSE
	// with the listening socket and newEvent   
	WSAEventSelect(socks[0], events[0], FD_ACCEPT | FD_CLOSE);


	if (bind(listenSock, (sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		printf("Error %d: Cannot associate a local address with server socket.", WSAGetLastError());
		return 0;
	}

	//Step 4: Listen request from client
	if (listen(listenSock, 10)) {
		printf("Error %d: Cannot place server socket in state LISTEN.", WSAGetLastError());
		return 0;
	}

	printf("Server started!\n");

	char sendBuff[BUFF_SIZE], recvBuff[BUFF_SIZE];
	SOCKET connSock;
	sockaddr_in clientAddr;
	int clientAddrLen = sizeof(clientAddr);
	int ret = 0, i;

	for (i = 1; i < WSA_MAXIMUM_WAIT_EVENTS; i++) {
		socks[i] = 0;
	}
	while (1) {
		//wait for network events on all socket
		index = WSAWaitForMultipleEvents(nEvents, events, FALSE, WSA_INFINITE, FALSE);
		if (index == WSA_WAIT_FAILED) {
			printf("Error %d: WSAWaitForMultipleEvents() failed\n", WSAGetLastError());
			break;
		}

		index = index - WSA_WAIT_EVENT_0;
		WSAEnumNetworkEvents(socks[index], events[index], &sockEvent);

		if (sockEvent.lNetworkEvents & FD_ACCEPT) {
			if (sockEvent.iErrorCode[FD_ACCEPT_BIT] != 0) {
				printf("FD_ACCEPT failed with error %d\n", sockEvent.iErrorCode[FD_READ_BIT]);
				break;
			}

			if ((connSock = accept(socks[index], (sockaddr *)&clientAddr, &clientAddrLen)) == SOCKET_ERROR) {
				printf("Error %d: Cannot permit incoming connection.\n", WSAGetLastError());
				break;
			}

			//Add new socket into socks array
			int i;
			if (nEvents == WSA_MAXIMUM_WAIT_EVENTS) {
				printf("\nToo many clients.");
				closesocket(connSock);
			}
			else
				for (i = 1; i < WSA_MAXIMUM_WAIT_EVENTS; i++)
					if (socks[i] == 0) {
						socks[i] = connSock;
						events[i] = WSACreateEvent();
						WSAEventSelect(socks[i], events[i], FD_READ | FD_CLOSE);
						nEvents++;
						break;
					}

			//reset event
			WSAResetEvent(events[index]);
		}

		if (sockEvent.lNetworkEvents & FD_READ) {
			//Receive message from client
			if (sockEvent.iErrorCode[FD_READ_BIT] != 0) {
				printf("FD_READ failed with error %d\n", sockEvent.iErrorCode[FD_READ_BIT]);
				break;
			}

			//ret = Receive(socks[index], recvBuff, BUFF_SIZE, 0);

			//Release socket and event if an error occurs
			if (ret <= 0) {
				closesocket(socks[index]);
				WSACloseEvent(events[index]);
				socks[index] = socks[nEvents-1];
				socks[nEvents - 1] = 0;
				events[index] = events[nEvents - 1];
				events[nEvents - 1] = 0;
				nEvents--;
			}
			else {									//echo to client
				memcpy(sendBuff, recvBuff, ret);
				//Send(socks[index], sendBuff, ret, 0);

				//reset event
				WSAResetEvent(events[index]);
			}
		}

		if (sockEvent.lNetworkEvents & FD_CLOSE) {
			if (sockEvent.iErrorCode[FD_CLOSE_BIT] != 0) {
				printf("FD_CLOSE failed with error %d\n", sockEvent.iErrorCode[FD_CLOSE_BIT]);
			}
			//Release socket and event
			closesocket(socks[index]);
			WSACloseEvent(events[index]);
			socks[index] = socks[nEvents - 1];
			socks[nEvents - 1] = 0;
			events[index] = events[nEvents - 1];
			events[nEvents - 1] = 0;
			nEvents--;
		}
	}
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