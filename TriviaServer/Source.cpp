#pragma comment (lib, "ws2_32.lib")

#include "WSAInitializer.h"
#include "Server.h"
#include <iostream>
#include <exception>
#include "MessageHandler.h"
#include <thread>
#include "Database.h"
#include <time.h>

using namespace std;

Server *myServer;

void serve()
{
	myServer = new Server();
	myServer->serve(54452);
}

void handle()
{
	MessageHandler mh;
	mh.handle();
}

void connector()
{
	system("TriviaConnector > null");
}

int main()
{
	std::srand(time(0));
	Database db;
	db.getQuestions(3, "hard", 13);
	WSAInitializer wsaInit;
	thread m(handle);
	thread t(connector);
	serve();
}