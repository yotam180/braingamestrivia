#pragma once

#include "User.h"
class User;

#include "Question.h"
class Question;

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
using namespace std;

class GameRoom
{
public:
	GameRoom(string name, int cat, int rounds, int timePerRound, int maxPlayers, string difficulty);
	~GameRoom();

	void playerLeave(User* user);
	void playerJoin(User* user);
	void close();

	void broadcastDetails();

	static int _lastroom;

	string name;
	vector<User*> users;
	int category;
	int gamestate;
	int maxPlayers;
	int currentRound;
	int maxRounds;
	string difficulty;
	int timePerQuestion; 
	int Id;
	int answerers;
	vector<Question> questions;
	map<string,int> formers;
	static const int
		WAITING = 0,
		STARTING_GAME = 1,
		IN_QUESTION = 2,
		IN_ANSWERS = 3,
		IN_PODIUM = 4;
};

