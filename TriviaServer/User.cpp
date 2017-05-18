#include "User.h"
#include "MessageHandler.h"

Scanner* User::getScanner()
{
	return scanner;
}
void User::setScanner(Scanner* s)
{
	scanner = s;
}

void User::logout()
{
	if (gameroom != nullptr)
	{
		gameroom->playerLeave(this);
	}
	gameroom = nullptr;
}

void User::setRoom(GameRoom * room)
{
	gameroom = room;
}

GameRoom * User::getRoom()
{
	return gameroom;
}

void User::clearRoom()
{
	gameroom->playerLeave(this);
	gameroom = nullptr;
}