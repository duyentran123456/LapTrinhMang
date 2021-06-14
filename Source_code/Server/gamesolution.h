#pragma once
// include function of game 

#include <string>
#include <vector>
using namespace std;

struct Question {
	string content;
	string ansA;
	string ansB;
	string ansC;
	string ansD;
	string ansTrue;
};
struct Assist {
	bool _50_50;
	bool _call_a_friend;
	bool _ask_the_audience;
	Assist() {
		this->_50_50 = true;
		this->_call_a_friend = true;
		this->_ask_the_audience = true;
	}
};


struct Account {
	string user;
	string password;
};