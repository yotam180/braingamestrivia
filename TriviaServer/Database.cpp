#include "Database.h"
#include "User.h"
class User;
#include "MessageHandler.h"
class MessageHandler;
#include <time.h>
#include <chrono>
using namespace std;
Database* Database::_instance;

/*
This function adds a new user entry to the database.
Parameters:
	string username - the username of the new user
	string password - the password of the new user
	string email - the email of the new user
Return value:
	int - the message that should be returned to the client
*/
int Database::signup(string username, string password, string email)
{
	// input validation
	if (username.length() < 6)
	{
		return Message::RESPONSE_SIGNUP_USERNAME_ILLEGAL;
	}
	if (password.length() < 6)
	{
		return Message::RESPONSE_SIGNUP_ILLEGAL_PASSWORD;
	}
	if (username.find_first_not_of(string("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_.")) != string::npos)
	{
		return Message::RESPONSE_SIGNUP_USERNAME_ILLEGAL;
	}
	if (password.find_first_not_of(string("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_.")) != string::npos)
	{
		return Message::RESPONSE_SIGNUP_ILLEGAL_PASSWORD;
	}

	// database validation
	bool istaken = false;
	char* err;
	stringstream sql;
	sql << "SELECT Users.Username FROM Users WHERE Users.Username='" << username << "';";
	if (sqlite3_exec(db, sql.str().c_str(),
		[](void* taken, int argc, char* argv[], char* types[]) -> int {
		*((bool*)taken) = true;
		return 0;
	}, &istaken, &err) == SQLITE_ABORT)
	{
		sqlite3_free(err);
		return Message::RESPONSE_SIGNUP_UNKNOWN_PROBLEM;
	}
	sqlite3_free(err);
	if (istaken)
	{
		return Message::RESPONSE_SIGNUP_USERNAME_TAKEN;
	}

	//user insertion
	sql.str(string());
	sql << "begin transaction; INSERT INTO Users(Username, Password, Email) VALUES ('" << username << "', '" << password << "', '" << email << "'); commit;";
	if (sqlite3_exec(db, sql.str().c_str(), nullptr, nullptr, &err) == SQLITE_OK)
	{
		sqlite3_free(err);
		return Message::RESPONSE_SIGNUP_SUCCESS;
	}
	sqlite3_free(err);
	return Message::RESPONSE_SIGNUP_UNKNOWN_PROBLEM;
}

/*
This method is used to authenticate a user using their credentials.
Parameters:
	string username - the username of the user
	string password - the password of the user.
Return value:
	int - the message that should be returned to the client.
*/
int Database::login(string username, string password, User** user)
{
	if (username.length() < 6)
	{
		return Message::RESPONSE_SIGNUP_USERNAME_ILLEGAL;
	}
	if (password.length() < 6)
	{
		return Message::RESPONSE_SIGNUP_ILLEGAL_PASSWORD;
	}
	if (username.find_first_not_of(string("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_.")) != string::npos)
	{
		return Message::RESPONSE_SIGNUP_USERNAME_ILLEGAL;
	}
	if (password.find_first_not_of(string("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_.")) != string::npos)
	{
		return Message::RESPONSE_SIGNUP_ILLEGAL_PASSWORD;
	}

	vector<User*>& users = MessageHandler::instance()->users;
	if (any_of(users.begin(), users.end(), [username](User* u) { return u->getUsername() == username; }))
	{
		return Message::RESPONSE_LOGIN_ALREADY_CONNECTED;
	}

	stringstream sql;
	sql << "SELECT Users.Username, Users.Password FROM Users WHERE Users.Username='" << username << "' AND Users.Password='" << password << "';";
	bool isok = false;
	char *err;
	if (sqlite3_exec(db, sql.str().c_str(),
		[](void* ok, int argc, char* argv[], char* types[]) -> int {
		*((bool*)ok) = true;
		return 0;
	}, &isok, &err) == SQLITE_ABORT)
	{
		sqlite3_free(err);
		return Message::RESPONSE_LOGIN_WRONG_CREDENTIALS;
	}
	sqlite3_free(err);
	if (isok)
	{
		User* entry = new User(username, password);
		MessageHandler::instance()->users.push_back(entry);
		*user = entry;
		return Message::RESPONSE_LOGIN_SUCCESS;
	}
	return Message::RESPONSE_LOGIN_WRONG_CREDENTIALS;
}

int Database::startGame()
{
	stringstream sql;
	//sql << "INSERT INTO Games (Status,StartTime) VALUES (0," <<duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() << ");";
	// Commented this out so there are no compiler errors
	return 0;
}

/*
This queries a list of questions from the database, and returns them as a vector.
Parameters:
	int amount - the number of questions we want to pull out of the database.
	string difficulty - "easy" for easy questions, "medium" for medium, or "hard" for not filtering questions
	int category - the category id of the questions.
Return value:
	vector<Question> - a vector of questions that were pulled of the database. If there were not enough
	questions with the required parameters, the returned vector's length will be smaller than the amount parameter.
*/
vector<Question> Database::getQuestions(int amount, string difficulty, int category)
{
	stringstream sql;
	sql << "select Question, Correct, Ans1, Ans2, Ans3, Category, Difficulty from Questions";
	if (difficulty != "hard")
	{
		sql << " where Difficulty='" << difficulty << "'";
		if (category > 8)
		{
			sql << " and Category='" << category << "'";
		}
	}
	else if (category > 8)
	{
		sql << " where Category='" << category << "'";
	}
	sql << " order by RANDOM() limit " << amount << ";";

	vector<Question> results;
	char *err;
	if (sqlite3_exec(db, sql.str().c_str(),
		[](void* ok, int argc, char* argv[], char* types[]) -> int {
		vector<Question>* r = (vector<Question>*)ok;
		Question q(string(argv[0]), string(argv[1]), string(argv[2]), string(argv[3]), string(argv[4]), atoi(argv[5]), string(argv[6]));
		r->push_back(q);
		return 0;
	}, &results, &err) == SQLITE_ABORT)
	{
		sqlite3_free(err);
		return vector<Question>();
	}
	sqlite3_free(err);
	return results;
}
