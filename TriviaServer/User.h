#pragma once
#include <string>
#include <WinSock2.h>
#include <Windows.h>
using namespace std;
#include "GameRoom.h"
class GameRoom;
#include "Scanner.h"

class User
{
public:
	User(string username, string password)
	{
		this->username = username;
		this->password = password;
	}

	string getUsername()
	{
		return username;
	}

	void clearRoom();
	Scanner* getScanner();
	void setScanner(Scanner* s);
	void logout();
	void setRoom(GameRoom* room);
	GameRoom* getRoom();

private:
	string username;
	string password;
	GameRoom* gameroom;
	Scanner* scanner;
};

