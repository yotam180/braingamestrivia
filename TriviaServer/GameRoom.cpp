#include "GameRoom.h"
#include "MessageHandler.h"
class MessageHandler;

int GameRoom::_lastroom = 0;

/*
Constructs a new GameRoom object with initial parameters:
Parameters:
	string name - the game title (name). Does not have to be uniqe as it doesn't serve as an identifier.
	int cat - category id of the game to be created.
	int rounds - questions number in the game that will be played.
	int timePerRound - time given to answer every question in the game, in seconds.
	int maxPlayers - maximum amount of players for the room
	string difficulty - "easy", "medium" or "hard"
Note: These parameters can be changed later.
*/
GameRoom::GameRoom(string name, int cat, int rounds, int timePerRound, int maxPlayers, string difficulty)
{
	this->name = name;
	this->category = cat;
	this->timePerQuestion = timePerRound;
	this->maxPlayers = maxPlayers;
	this->difficulty = difficulty;
	this->maxRounds = rounds;
	this->Id = ++_lastroom;
	this->gamestate = GameRoom::WAITING;
	cout << "A new game room has been created: " << name << endl;
}

/*
Deallocates a GameRoom.
*/
GameRoom::~GameRoom()
{
	for (int i = 0; i < users.size(); i++)
	{
		users[i]->clearRoom();
	}
	MessageHandler::instance()->tasks.erase(remove_if(MessageHandler::instance()->tasks.begin(), MessageHandler::instance()->tasks.end(), [this](TimedTask x) -> bool { return x.param == this; }), MessageHandler::instance()->tasks.end());
}

/*
Removes a player from a GameRoom, updating both the GameRoom object and the User object.
*/
void GameRoom::playerLeave(User* user)
{
	user->setRoom(nullptr);
	cout << user->getUsername() << " has left room #" << Id << endl;
	this->formers[user->getUsername()] = user->getScore();
	users.erase(std::remove(users.begin(), users.end(), user), users.end());
	broadcastDetails();
	if (users.size() == 0)
	{
		cout << "Room #" << Id << " has been emptied of players. Room is deleted." << endl;
		vector<GameRoom*>& rooms = MessageHandler::instance()->gamerooms;
		rooms.erase(std::remove_if(rooms.begin(), rooms.end(), [this](GameRoom* r) { return r->Id == this->Id; }), rooms.end());
		delete this;
	}
}

/*
Adds a player to a GameRoom, updating both the GameRoom object and the User object.
*/
void GameRoom::playerJoin(User * user)
{
	user->setRoom(this);
	users.push_back(user);
	broadcastDetails();
}

/*
Closes a GameRoom.
*/
void GameRoom::close()
{
	while (users.size() > 0)
	{
		users[0]->getScanner()->writeInt(116);
		playerLeave(users[0]);
	}
}

/*
Updates all players in the room with the room details.
*/
void GameRoom::broadcastDetails()
{
	for (int i = 0; i < users.size(); i++)
	{
		User* user = users[i];
		Scanner& scanner = *user->getScanner();
		scanner.writeInt(Message::BROADCAST_ROOM_DETAILS);

		scanner.writeStr(name);
		scanner.writeInt(category);
		scanner.writeStr(difficulty);
		scanner.writeInt(maxPlayers);
		scanner.writeInt(maxRounds);
		scanner.writeInt(timePerQuestion);
		scanner.writeInt(users.size());
		scanner.writeInt(i == 0 ? 1 : 0);
		for (int j = 0; j < users.size(); j++)
		{
			scanner.writeStr(users[j]->getUsername());
		}
	}
}
