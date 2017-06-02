#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include "Scanner.h"

class Message
{
public:
	Message(int type, Scanner* soc);
	~Message();


	static const int
		REQUEST_LOGIN = 200,//
		RESPONSE_LOGIN_SUCCESS = 1020,//
		RESPONSE_LOGIN_WRONG_CREDENTIALS = 1021,//
		RESPONSE_LOGIN_ALREADY_CONNECTED = 1022,//
		REQUEST_LOGOUT = 201,//
		REQUEST_SIGNUP = 203,//
		RESPONSE_SIGNUP_SUCCESS = 1040,//
		RESPONSE_SIGNUP_ILLEGAL_PASSWORD = 1041,//
		RESPONSE_SIGNUP_USERNAME_TAKEN = 1042,//
		RESPONSE_SIGNUP_USERNAME_ILLEGAL = 1043,//
		RESPONSE_SIGNUP_UNKNOWN_PROBLEM = 1044,//
		REQUEST_ROOMLIST = 205,							// CANCELED - Auto sent
		BROADCAST_ROOMLIST = 106,//
		BROADCAST_PLAYERLIST = 107,//
		REQUEST_PLAYERLIST_IN_ROOM = 207,				// 
		BROADCAST_ROOM_DETAILS = 108,//
		REQUEST_JOIN_ROOM = 209,//
		RESPONSE_ROOM_JOINED = 1100,//
		RESPONSE_ROOM_FULL = 1101,//
		RESPONSE_ROOM_JOIN_ERROR = 1102,//
		REQUEST_LEAVE_ROOM = 211,//
		RESPONSE_ROOM_LEFT = 112,//
		REQUEST_CREATE_ROOM = 213,//
		RESUEST_EDIT_ROOM = 214,//
		RESPONSE_ROOM_CREATED = 1140,//
		RESPONSE_ROOM_CREATION_FAILED = 1141,//
		REQUEST_CLOSE_ROOM = 215,//
		BROADCAST_ROOM_CLOSED = 116,//
		BROADCAST_GAME_DETAILS = 117, // What the heck is that? Not even in the protocol..
		REQUEST_START_GAME = 217,//
		BROADCAST_GAME_STARTING = 1170,//
		RESPONSE_GAMESTART_TOO_FEW_PLAYERS = 1171,//
		BROADCAST_NEW_QUESTION = 118,//
		REQUEST_ANSWER = 219,//
		RESPONSE_ANSWER_CORRECT = 1200, //
		RESPONSE_ANSWER_WRONG = 1201,//
		BROADCAST_QUESTION_STATUS = 1301,
		BROADCAST_QUESTION_ENDED = 130,
		BROADCAST_GAME_ENDED = 121,
		REQUEST_LEAVE_GAME = 222,
		REQUEST_USER_SCORES = 223,
		RESPONSE_BEST_SCORES = 124,
		REQUEST_SELF_SCORE = 225,
		RESPONSE_SELF_SCORE = 126,
		REQUEST_LEAVE_APPLICATION = 299;

	int getType();

	Scanner* getScanner();

private:
	int type;
	Scanner* sender;
};
