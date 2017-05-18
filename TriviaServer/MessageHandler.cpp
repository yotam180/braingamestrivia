#include "MessageHandler.h"
#include "Scanner.h"
#include "Database.h"
#include <time.h>
#include <chrono>
#include <sstream>
using namespace chrono;

MessageHandler* MessageHandler::_instance;
bool MessageHandler::_active = true;
typedef GameRoom room;

long long time()
{
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void MessageHandler::handle()
{
	long long lastBroadcast = time();
	while (_active)
	{
		if (time() - lastBroadcast > 1500)
		{
			lastBroadcast = time();
			stringstream rlm;
			rlm << to_string(Message::BROADCAST_ROOMLIST);
			rlm << "##" << to_string(gamerooms.size());
			for (int i = 0; i < gamerooms.size(); i++)
			{
				room& r = *gamerooms[i];
				rlm << "##"
					<< r.Id << "##"
					<< r.name << "##"
					<< r.category << "##"
					<< r.difficulty << "##"
					<< r.gamestate << "##"
					<< r.maxPlayers << "##"
					<< r.users.size() << "##"
					<< r.users[0]->getUsername() << "##"
					<< r.maxRounds << "##"
					<< r.timePerQuestion;
			}
			stringstream u;
			u << to_string(Message::BROADCAST_PLAYERLIST) << "##" << to_string(users.size());
			for (int i = 0; i < users.size(); i++)
			{
				u << "##" << users[i]->getUsername();
				if (users[i]->getRoom() == nullptr)
				{
					users[i]->getScanner()->writeStr(rlm.str().c_str());
				}
			}
			for (int i = 0; i < users.size(); i++)
			{
				if (users[i]->getRoom() == nullptr)
				{
					users[i]->getScanner()->writeStr(u.str().c_str());
				}
			}
		}
		if (messageQueue.size() == 0) 
		{
			continue;
		}
		if (!_active)
		{
			return;
		}
		unique_lock<mutex> l(m);
		Message msg = messageQueue.front();
		messageQueue.pop();
		l.unlock();

		Scanner& scanner = *msg.getScanner();
		if (msg.getType() == Message::REQUEST_SIGNUP)
		{
			string username = scanner.nextStr();
			string password = scanner.nextStr();
			string email = scanner.nextStr();
			int response = Database::instance()->signup(username, password, email);
			scanner.writeInt(response);
		}
		if (msg.getType() == Message::REQUEST_LOGIN)
		{
			string username = scanner.nextStr();
			string password = scanner.nextStr();
			User* user;
			int response = Database::instance()->login(username, password, &user);
			if (response == Message::RESPONSE_LOGIN_SUCCESS)
			{
				user->setScanner(msg.getScanner());
			}
			scanner.writeInt(response);
		}
		if (msg.getType() == Message::REQUEST_LEAVE_APPLICATION
			|| msg.getType() == Message::REQUEST_LOGOUT)
		{
			for (int i = 0; i < users.size(); i++)
			{
				if (users[i]->getScanner() == msg.getScanner())
				{
					users[i]->logout();
					User* u = users[i];
					users.erase(users.begin() + i);
					delete u;
					break;
				}
			}
		}
		if (msg.getType() == Message::REQUEST_CREATE_ROOM)
		{
			string roomname = scanner.nextStr();
			int numPlayers = scanner.nextInt();
			int numRounds = scanner.nextInt();
			int roundTime = scanner.nextInt();
			int category = scanner.nextInt();
			string difficulty = scanner.nextStr(); 
			if (gamerooms.size() > 100
				|| numPlayers < 2 || numPlayers > 10
				|| ((category > 27 || category < 9) && category)
				|| (difficulty != "easy" && difficulty != "hard" && difficulty != "medium")
				|| numRounds < 1 || numRounds > 20
				|| roundTime < 5 || roundTime > 60
				|| roomname.length() < 1 || roomname.length() > 30)
			{
				scanner.writeInt(Message::RESPONSE_ROOM_CREATION_FAILED);
				goto RET;
			}
			room* room = new GameRoom(roomname, category, numRounds, roundTime, numPlayers, difficulty);
			gamerooms.push_back(room);
			User* user = getUser(&scanner);
			room->playerJoin(user);
			scanner.writeInt(Message::RESPONSE_ROOM_CREATED);
			scanner.writeInt(room->Id);
		}
		if (msg.getType() == Message::RESUEST_EDIT_ROOM)
		{
			int numPlayers = scanner.nextInt();
			int numRounds = scanner.nextInt();
			int roundTime = scanner.nextInt();
			int category = scanner.nextInt();
			string difficulty = scanner.nextStr();
			User* user = getUser(&scanner);
			room* r = user->getRoom();

			if (r == nullptr || r->users[0] != user
				|| numPlayers < 2 || numPlayers < r->users.size() || numPlayers > 10
				|| ((category > 27 || category < 9) && category)
				|| (difficulty != "easy" && difficulty != "hard" && difficulty != "medium")
				|| numRounds < 1 || numRounds > 20
				|| roundTime < 5 || roundTime > 60)
			{
				r->broadcastDetails();
				goto RET;
			}
			r->maxPlayers = numPlayers;
			r->maxRounds = numRounds;
			r->timePerQuestion = roundTime;
			r->category = category;
			r->difficulty = difficulty;
			r->broadcastDetails();
		}
		if (msg.getType() == Message::REQUEST_LEAVE_ROOM)
		{
			User* user = getUser(&scanner);
			if (user->getRoom() != nullptr)
			{
				user->clearRoom();
				scanner.writeInt(Message::RESPONSE_ROOM_LEFT);
			}
		}
		/*if (msg.getType() == Message::REQUEST_ROOMLIST) // Roomlist is no longer sent by demand but by timeout and globally.
		{
			int roomcount = gamerooms.size();
			scanner.writeInt(Message::RESPONSE_ROOMLIST);
			scanner.writeInt(roomcount);
			for (int i = 0; i < roomcount; i++)
			{
				scanner.writeInt(gamerooms[i]->Id);
				scanner.writeStr(gamerooms[i]->name);
				scanner.writeInt(gamerooms[i]->category);
				scanner.writeStr(gamerooms[i]->difficulty);
				scanner.writeInt(gamerooms[i]->gamestate);
				scanner.writeInt(gamerooms[i]->maxPlayers);
				scanner.writeInt(gamerooms[i]->users.size());
			}
		}*/ 
		if (msg.getType() == Message::REQUEST_JOIN_ROOM)
		{
			int roomid = scanner.nextInt();
			room* r = getRoom(roomid);
			User* user = getUser(&scanner);
			if (r == nullptr || user == nullptr || user->getRoom() != nullptr)
			{
				scanner.writeInt(Message::RESPONSE_ROOM_JOIN_ERROR);
			}
			else if (r->users.size() >= r->maxPlayers)
			{
				scanner.writeInt(Message::RESPONSE_ROOM_FULL);
			}
			else
			{
				r->playerJoin(user);
				scanner.writeInt(Message::RESPONSE_ROOM_JOINED);
			}
		}
		if (msg.getType() == Message::REQUEST_CLOSE_ROOM)
		{
			User* user = getUser(&scanner);
			room* r = user->getRoom();
			if (r->users[0] != user)
			{
				goto RET;
			}
			r->close();
		}
		if (msg.getType() == Message::REQUEST_START_GAME)
		{
			User* u = getUser(&scanner);
			room* r = u->getRoom();
			if (r == nullptr || r->users.size() < 2)
			{
				scanner.writeInt(Message::RESPONSE_GAMESTART_TOO_FEW_PLAYERS);
				goto RET;
			}
			scanner.writeInt(Message::BROADCAST_GAME_STARTING);
		}
		RET:
		
		if (msg.getType() != Message::REQUEST_LEAVE_APPLICATION) scanner.release();
	}
}

mutex & MessageHandler::getQueueLocker()
{
	return m;
}

User * MessageHandler::getUser(Scanner* s)
{
	auto i = find_if(users.begin(), users.end(), [s](User* u) { return u->getScanner() == s; });
	return i[0];
}

room * MessageHandler::getRoom(int id)
{
	for (int i = 0; i < gamerooms.size(); i++)
	{
		if (gamerooms[i]->Id == id) return gamerooms[i];
	}
	return nullptr;
}