#pragma once
#include "sqlite3.h"
#include <iostream>
#include <exception>
#include <sstream>
#include "Message.h"
class Message;
#include "Question.h"
class Question;
#include "User.h"
class User;

using namespace std;

class Database
{
public:
	Database()
	{
		int rc = sqlite3_open("database.db", &db);
		if (rc)
		{
			throw exception((string("Database error: ") + string(sqlite3_errmsg(db))).c_str());
		}
		_instance = this;
	}
	~Database()
	{
		sqlite3_close(db);
	}
	static Database* instance()
	{
		return _instance;
	}

	int signup(string username, string password, string email);

	int login(string username, string password, User** u);
	vector<Question> getQuestions(int amount, string difficulty, int category);

private:
	sqlite3 *db;
	static Database* _instance;
};

