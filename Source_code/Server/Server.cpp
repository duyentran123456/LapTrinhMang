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
#define CLIENT_DISCONNECT 0

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

using namespace std;

struct ClientInfo {
	SOCKET socket;
	sockaddr_in clientAddr;// address of client
	string userClient; // user client logined
	int statusLogin;// logined is 1, else 0
	int statusInGame;
	Question listQues[15];
	int score;
	Assist assist;

	bool getQuestions() {
		int randArr[15];
		for (int i = 0; i < 15; i++) {
			randArr[i] = rand() % QUESTION_NUM;
		}

		for (int i = 0; i < 15; i++) {
			if (!getQuestion(randArr[i], &listQues[i]))
				return false;
		}

		return true;
	}

};

// function prototype
string recv(SOCKET &currSoc);
void send(SOCKET &currSoc, char req[]);
string solveRequest(ClientInfo*, char[]);
string signUp(ClientInfo*, char*);
string logIn(ClientInfo*, char*);
string logOut(ClientInfo*);
string start(ClientInfo*, char*);
string answer(ClientInfo*, char*);
string assist(ClientInfo*, char*);
string quit(ClientInfo*, char*);
void communicating(ClientInfo *);

int main(int argc, char* argv[])
{
	DWORD		nEvents = 0;
	DWORD		index;
	ClientInfo* 	clientInfos[WSA_MAXIMUM_WAIT_EVENTS];
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

	ClientInfo* clientInfo = new ClientInfo;
	memset(clientInfo, 0, sizeof(*clientInfo));
	clientInfo->socket = listenSock;
	clientInfos[0] = clientInfo;
	events[0] = WSACreateEvent(); //create new events
	nEvents++;

	// Associate event types FD_ACCEPT and FD_CLOSE
	// with the listening socket and newEvent   
	WSAEventSelect(clientInfos[0]->socket, events[0], FD_ACCEPT | FD_CLOSE);


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
	int ret, i;
	for (i = 1; i < WSA_MAXIMUM_WAIT_EVENTS; i++) {
		clientInfos[i] = 0;
	}
	while (1) {
		//wait for network events on all socket
		index = WSAWaitForMultipleEvents(nEvents, events, FALSE, WSA_INFINITE, FALSE);
		if (index == WSA_WAIT_FAILED) {
			printf("Error %d: WSAWaitForMultipleEvents() failed\n", WSAGetLastError());
			break;
		}

		index = index - WSA_WAIT_EVENT_0;
		WSAEnumNetworkEvents(clientInfos[index]->socket, events[index], &sockEvent);

		if (sockEvent.lNetworkEvents & FD_ACCEPT) {
			if (sockEvent.iErrorCode[FD_ACCEPT_BIT] != 0) {
				printf("FD_ACCEPT failed with error %d\n", sockEvent.iErrorCode[FD_READ_BIT]);
				break;
			}

			if ((connSock = accept(clientInfos[index]->socket, (sockaddr *)&clientAddr, &clientAddrLen)) == SOCKET_ERROR) {
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
					if (clientInfos[i] == 0) {
						ClientInfo* clientInfo = new ClientInfo;
						memset(clientInfo, 0, sizeof(*clientInfo));
						clientInfo->socket = connSock;
						clientInfo->clientAddr = clientAddr;

						clientInfos[i] = clientInfo;
						events[i] = WSACreateEvent();
						WSAEventSelect(clientInfos[i]->socket, events[i], FD_READ | FD_CLOSE);
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
			try {
				communicating(clientInfos[index]);
				WSAResetEvent(events[index]);
			}
			catch (int err) {
				if (err == SOCKET_ERROR || err == CLIENT_DISCONNECT) {
					cout << "Client disconnect!\n";
					//string code = quit(clientInfo[index]);
					closesocket(clientInfos[index]->socket);
					WSACloseEvent(events[index]);
					clientInfos[index] = clientInfos[nEvents - 1];
					clientInfos[nEvents - 1] = 0;
					events[index] = events[nEvents - 1];
					events[nEvents - 1] = 0;
					nEvents--;
				}
			}
		}
		if (sockEvent.lNetworkEvents & FD_CLOSE) {
			if (sockEvent.iErrorCode[FD_CLOSE_BIT] != 0) {
				printf("FD_CLOSE failed with error %d\n", sockEvent.iErrorCode[FD_CLOSE_BIT]);
			}
			//Release socket and event
			//string code = quit(clientInfos[index]);
			closesocket(clientInfos[index]->socket);
			WSACloseEvent(events[index]);
			clientInfos[index] = clientInfos[nEvents - 1];
			clientInfos[nEvents - 1] = 0;
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

string solveRequest(ClientInfo *clientInfo, char str[]) {
	string header, body;
	for (int i = 0; i < 4; i++) header.push_back(str[i]);
	for (int i = 5; i < strlen(str); i++) body.push_back(str[i]);
	string response;
	/*if (strcmp(&header[0], USER) == 0) {
		response = login(clientInfo, &body[0]);
	}
	else if (strcmp(&header[0], POST) == 0) {
		response = postMess(loginStruct, &body[0]);
	}
	else if (strcmp(&header[0], QUIT) == 0) {
		response = quit(loginStruct);
	}
	else response = UNFORMAT_REQUEST;*/
	return response;
}

vector<string> splitRequest(string s) {
	vector<string> res;
	string delimiter = "\r\n";
	size_t pos = 0;
	string token;
	while ((pos = s.find(delimiter)) != string::npos) {
		token = s.substr(0, pos);
		res.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	if (s.length()>0) res.push_back(s);
	return res;
}

void communicating(ClientInfo *clientInfo) {
	// receive message from client
	string buff;
	string res;
	buff = recv(clientInfo->socket);
	if (buff.size() == 0) {
		//string tmp = quit(clientInfo);
		return;
	}

	buff[buff.size() - 2] = 0;
	// copy message without "\r\n"
	string tmp;
	for (int i = 0; i < buff.size() - 2; i++) tmp.push_back(buff[i]);
	vector<string> reqs = splitRequest(buff);
	// solve each string of vector string request
	for (int i = 0; i < reqs.size(); i++) {
		string req = reqs[i];
		res = solveRequest(clientInfo, &req[0]);
		res.append("\r\n");
		// send response after append "\r\n" to client
		send(clientInfo->socket, &res[0]);
	}
}

/*
	req: SIGNUP [username] [password]
	res:
		success: 30 SIGNUP
		failure: [err-code] [detail]
*/
string signUp(ClientInfo* clientInfo, char* body) {
	string response;

	string bodyStr = body, username, password;
	int start = 0;
	int end = bodyStr.find(" ");
	if (end == -1) return BAD_REQUEST;
	username = bodyStr.substr(start, end - start);

	start = end + 1;
	end = bodyStr.find(" ", start);
	if (end != -1) return BAD_REQUEST;
	end = bodyStr.length();
	password = bodyStr.substr(start, end - start);

	fstream AccountsData("accounts.txt");

	string account, username_account;
	while (getline(AccountsData, account)) {
		username_account = account.substr(0, account.find(" "));
		if (username == username_account) {
			AccountsData.close();
			return "USERNAME_EXISTED";
		}
	}

	AccountsData << username << " " << password << endl;
	AccountsData.close();

	response.append(SUCCESS).append(" ").append(SIGNUP);

	return response;
}
string logIn(ClientInfo* clientInfo, char* body) {
	string response;
	return response;
}

/*
	req: LOGOUT
	res:
		success: 30 LOGOUT
		failure: [err-code] [detail]
*/
string logOut(ClientInfo* clientInfo) {
	string response;

	if (clientInfo->statusLogin) {
		clientInfo->statusLogin = false;
		response.append(SUCCESS).append(" ").append(LOGOUT);
	}
	else {
		response = "USER NOT LOGIN";
	}

	return response;
}
string start(ClientInfo* clientInfo, char* body) {
	string response;
	return response;
}


/*
	req: ANSWER [option]
	res: 
		success: RESULT [true/false]
		failure: [err-code] [detail]
*/
string answer(ClientInfo* clientInfo, char* body) {
	string response;

	if (clientInfo->statusLogin == 0) return "USER NOT LOGIN";
	if (clientInfo->statusInGame == false) return "USER NOT IN GAME";
	
	Question currentQuestion = clientInfo->listQues[clientInfo->score];

	if (currentQuestion.ansTrue == body) response.append(SUCCESS).append(" ").append("TRUE");
	else response.append(SUCCESS).append(" ").append("FALSE");

	return response;
}

/*
	req: ASSIST [option]
	res:
		success:
			ASSIST 50-50 [wrong-option-1] [wrong-option-2]
			ASSIST friend [pro-option]
			ASSIST audience [percen-1] [percent-2] [percent-3] [percent-4]
		failure:
			[err-code] [detail]
*/
string assist(ClientInfo* clientInfo, char* body) {
	string response;

	if (clientInfo->statusLogin == 0) return "USER NOT LOGIN";
	if (clientInfo->statusInGame == false) return "USER NOT IN GAME";

	string option = body;
	Question currentQuestion = clientInfo->listQues[clientInfo->score];

	if (option == "5050") {
		if (clientInfo->assist._50_50) {
			clientInfo->assist._50_50 = false;

			string options[4] = { "A" , "B", "C", "D" };
			int wrong1, wrong2, right, random;
			for(int i = 0; i < 4; i++) 
				if (options[i] == currentQuestion.ansTrue) {
					right = i;
					break;
				}
			random = rand() % 4;
			while (random == right) random = rand() % 4;
			wrong1 = random;
			while (random == right || random == wrong1) random = rand() % 4;
			wrong2 = random;

			response.append(SUCCESS).append(" ").append(ASSIST).append(" ")
				.append(options[wrong1]).append(" ").append(options[wrong2]);

			return response;
		}
		else {
			return "ASSISTANCE USED";
		}
	}
	else if (option == "friend") {
		if (clientInfo->assist._call_a_friend) {
			clientInfo->assist._call_a_friend = false;

			string options[4] = { "A" , "B", "C", "D" };
			int random = rand() % 100;
			string friendOption = random < 80 ? currentQuestion.ansTrue : options[random % 4];

			response.append(SUCCESS).append(" ").append(ASSIST).append(" ").append(friendOption);

			return response;
		}
		else {
			return "ASSISTANCE USED";
		}
	}
	else {
		return BAD_REQUEST;
	}
}
string quit(ClientInfo* clientInfo, char* body) {
	string response;
	return response;
}
