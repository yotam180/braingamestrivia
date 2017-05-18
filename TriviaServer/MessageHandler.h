#pragma once
#include "User.h"
class User;

#include "GameRoom.h"
class GameRoom;

#include "Message.h"
class Message;

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <queue>
using namespace std;

class MessageHandler
{
public:
	vector<User*> users;
	vector<GameRoom*> gamerooms;
	queue<Message> messageQueue;
	mutex m;

	static MessageHandler* instance()
	{
		return _instance;
	}
	MessageHandler()
	{
		_instance = this;
	}
	~MessageHandler() {}

	void handle();
	mutex& getQueueLocker();
	static bool _active;
	User* getUser(Scanner* s);
	GameRoom* getRoom(int id);

private:
	static MessageHandler* _instance;
};

