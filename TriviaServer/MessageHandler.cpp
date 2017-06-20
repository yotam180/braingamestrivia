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
int getScore4Question(long long start, long long anstime, int questionTime)
{
	int interval = (anstime - start) / 1000;
	return (questionTime - interval) * 10 / questionTime;
}

void MessageHandler::handle()
{
	long long lastBroadcast = time();
	while (_active)
	{
		///
		///	TIMED TASKS HANDLING
		///
		for (int i = 0; i < tasks.size(); i++)
		{
			TimedTask& task = tasks[i];
			if (task.timeout < time())
			{
				if (task.type == TimedTask::START_GAME)
				{
					room* r = (room*)task.param;
					if (r == nullptr) goto ABORT_TASK;
					r->currentRound = 0;
					r->answerers = 0;
					r->gamestate = GameRoom::IN_QUESTION;

					r->questions[r->currentRound].time = time();
					for (int i = 0; i < r->users.size(); i++)
					{
						r->users[i]->last_answer = 0;
						r->users[i]->getScanner()->writeInt(Message::BROADCAST_NEW_QUESTION);
						r->users[i]->getScanner()->writeStr(r->questions[r->currentRound].question);
						r->users[i]->getScanner()->writeStr(r->questions[r->currentRound].ans1);
						r->users[i]->getScanner()->writeStr(r->questions[r->currentRound].ans2);
						r->users[i]->getScanner()->writeStr(r->questions[r->currentRound].ans3);
						r->users[i]->getScanner()->writeStr(r->questions[r->currentRound].ans4);
					}
					tasks.push_back(TimedTask(time() + r->timePerQuestion * 1000, TimedTask::END_QUESTION, r));
				}
				if (task.type == TimedTask::END_QUESTION)
				{
					room* r = (room*)task.param;
					if (r == nullptr) goto ABORT_TASK;
					r->gamestate = GameRoom::IN_ANSWERS;
					// Give scores & shit like this
					for (int i = 0; i < r->users.size(); i++)
					{
						r->users[i]->last_answer = 0;
						r->users[i]->getScanner()->writeInt(Message::BROADCAST_QUESTION_ENDED);
						r->users[i]->getScanner()->writeInt(r->questions[r->currentRound].correctAnswer);
						r->users[i]->getScanner()->writeInt(r->users[i]->score);
						int count = 1;
						for (int j = 0; j < r->users.size(); j++)
						{
							if (r->users[j]->getScore() > r->users[i]->getScore())
								count++;
						}
						for (std::map<string, int>::iterator it = r->formers.begin(); it != r->formers.end(); ++it)
						{
							if (it->second > r->users[i]->getScore())
							{
								count++;
							}
						}
						r->users[i]->getScanner()->writeInt(count);
						int amount = 0;
						for (int j = 0; j < r->users.size(); j++)
						{
							amount++;
						}
						for (std::map<string, int>::iterator it = r->formers.begin(); it != r->formers.end(); ++it)
						{
							amount++;
						}
						r->users[i]->getScanner()->writeInt(amount);
						/////////// TODO Bom send the scores of the logged-out players too
						for (int j = 0; j < r->users.size(); j++) 
						{
							r->users[i]->getScanner()->writeStr(r->users[j]->getUsername());
							r->users[i]->getScanner()->writeInt(r->users[j]->getScore());
						}
						for (std::map<string, int>::iterator it = r->formers.begin(); it != r->formers.end(); ++it)
						{
							r->users[i]->getScanner()->writeStr(it->first);
							r->users[i]->getScanner()->writeInt(it->second);
						}
					}
					if (r->currentRound < r->maxRounds - 1)
					{
						tasks.push_back(TimedTask(time() + 5000, TimedTask::END_ANSWERS, r));
					}
					else
					{
						tasks.push_back(TimedTask(time() + 5000, TimedTask::END_GAME, r));
					}
				}
				if (task.type == TimedTask::END_ANSWERS)
				{
					room* r = (room*)task.param;
					if (r == nullptr) goto ABORT_TASK;
					r->currentRound++;
					r->answerers = 0;
					r->gamestate = GameRoom::IN_QUESTION;

					r->questions[r->currentRound].time = time();
					for (int i = 0; i < r->users.size(); i++)
					{
						r->users[i]->last_answer = 0;
						r->users[i]->getScanner()->writeInt(Message::BROADCAST_NEW_QUESTION);
						r->users[i]->getScanner()->writeStr(r->questions[r->currentRound].question);
						r->users[i]->getScanner()->writeStr(r->questions[r->currentRound].ans1);
						r->users[i]->getScanner()->writeStr(r->questions[r->currentRound].ans2);
						r->users[i]->getScanner()->writeStr(r->questions[r->currentRound].ans3);
						r->users[i]->getScanner()->writeStr(r->questions[r->currentRound].ans4);
					}
					tasks.push_back(TimedTask(time() + r->timePerQuestion * 1000, TimedTask::END_QUESTION, r));
				}
				if (task.type == TimedTask::END_GAME)
				{
					// What the heck should I do here?
					room* r = (room*)task.param;
					if (r == nullptr) goto ABORT_TASK;
					r->gamestate = GameRoom::WAITING;
					for (int i = 0; i < r->users.size(); i++)
					{
						r->users[i]->getScanner()->writeInt(Message::BROADCAST_GAME_ENDED);
						r->users[i]->getScanner()->writeInt(r->users[i]->score);
						int count = 1;
						for (int j = 0; j < r->users.size(); j++)
						{
							if (r->users[j]->getScore() > r->users[i]->getScore())
								count++;
						}
						r->users[i]->getScanner()->writeInt(count);
						int amount = 0;
						for (int j = 0; j < r->users.size(); j++)
						{
							amount++;
						}
						for (std::map<string, int>::iterator it = r->formers.begin(); it != r->formers.end(); ++it)
						{
							amount++;
						}
						r->users[i]->getScanner()->writeInt(amount);
						/////////// TODO Bom send the scores of the logged-out players too
						for (int j = 0; j < r->users.size(); j++)
						{
							r->users[i]->getScanner()->writeStr(r->users[j]->getUsername());
							r->users[i]->getScanner()->writeInt(r->users[j]->getScore());
						}
						for (std::map<string, int>::iterator it = r->formers.begin(); it != r->formers.end(); ++it)
						{
							r->users[i]->getScanner()->writeStr(it->first);
							r->users[i]->getScanner()->writeInt(it->second);
						}
					}
				}
				ABORT_TASK:
				tasks.erase(tasks.begin() + i);
				i--;
			}
		}


		///
		///	BROADCASTING
		///
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


		///
		///	MESSAGE QUEUE HANDLING
		///
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
			msg.getScanner()->close();
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
			if (r == nullptr || user == nullptr || user->getRoom() != nullptr || r->gamestate != GameRoom::WAITING)
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
			if (r == nullptr || r->users.size() < 1 || r->users[0] != u)
			{
				scanner.writeInt(Message::RESPONSE_GAMESTART_TOO_FEW_PLAYERS);
				goto RET;
			}
			r->questions = Database::instance()->getQuestions(r->maxRounds, r->difficulty, r->category);
			r->maxRounds = r->questions.size();

			r->gamestate = GameRoom::STARTING_GAME;
			r->formers.clear();
			
			for (int i = 0; i < r->users.size(); i++)
			{
				r->users[i]->score = 0;
				r->users[i]->last_answer = 0;
				r->users[i]->getScanner()->writeInt(Message::BROADCAST_GAME_STARTING);
			}
			tasks.push_back(TimedTask(time() + 5000, TimedTask::START_GAME, r));
		}
		if (msg.getType() == Message::REQUEST_ANSWER)
		{
			int answer = msg.getScanner()->nextInt();
			User* u = getUser(msg.getScanner());
			if (u->last_answer) 
			{
				goto RET;
			}
			room* r = u->getRoom();
			u->last_answer = answer;
			if (r->questions[r->currentRound].correctAnswer == answer) 
			{
				u->incScore(getScore4Question(r->questions[r->currentRound].time, time(), r->timePerQuestion));
				msg.getScanner()->writeInt(Message::RESPONSE_ANSWER_CORRECT);
				msg.getScanner()->writeInt(u->getScore());
			}
			else
			{
				msg.getScanner()->writeInt(Message::RESPONSE_ANSWER_WRONG);
			}
			r->answerers++;
			for (int i = 0; i < r->users.size(); i++) {
				r->users[i]->getScanner()->writeInt(Message::BROADCAST_QUESTION_STATUS);
				r->users[i]->getScanner()->writeInt(r->answerers);
				r->users[i]->getScanner()->writeInt(r->users.size());
			}
		}

		RET:
		if (msg.getType() != Message::REQUEST_LEAVE_APPLICATION)
		{

			//getUser(msg.getScanner())->getRoom()
			scanner.release();
			
		}
		else delete msg.getScanner();
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