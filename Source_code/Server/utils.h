#pragma once
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "gamesolution.h"

#define QUESTION_DATA_LOCATION "questions.csv"
#define QUESTION_NUM 100

bool getQuestion(int id, Question* question) {

	std::ifstream fin(QUESTION_DATA_LOCATION);
	if (fin.fail()) {
		cout << "Cannot access database" << endl;
		return false;
	}

	vector<string> row;
	string line, word, discard;
	int found = 0;

	while (!fin.eof()) {
		row.clear();
		getline(fin, line);
		stringstream ss(line);

		while (ss >> std::ws) {
			if (ss.peek() == '"') {
				ss >> std::quoted(word);
				std::getline(ss, discard, ',');
			}
			else {
				std::getline(ss, word, ',');
			}
			row.push_back(word);
		}

		if (stoi(row[0]) == id) {
			found = 1;
			question->content = row[1];
			question->ansA = row[2];
			question->ansB = row[3];
			question->ansC = row[4];
			question->ansD = row[5];
			question->ansTrue = row[6];
		}
	}

	fin.close();

	if (found == 0) {
		return false;
	}

	return true;
}
