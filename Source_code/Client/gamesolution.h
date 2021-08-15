#pragma once
// include function of game 

#include <string>
#include <vector>
#include "fstream"
#include "time.h"
#include "stdio.h"
#include "iostream"
#include "utils.h"
#include "ws2tcpip.h"
#include "winsock2.h"
#define BUFF_SIZE 2048

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
#define UNFORMAT_REQUEST "UNFORMAT_REQUEST"
using namespace std;

enum ResponseCode {
	SUCCESS = 30,
	// Login
	USER_LOGINED = 25,
	ACCOUNT_LOCKED = 26,
	INCORRECT_ACCOUNT = 27,
	NO_LOGIN = 28,

	SERVER_ERROR = 10,
	BAD_REQUEST = 20,
	CANNOT_USE_ASSIST = 21,
	CANNOT_START_GAME = 22,
	CANNOT_ANSWER = 23,
	CANNOT_QUIT = 24,
	STARTED = 29,
};