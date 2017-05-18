#include "Server.h"
#include <exception>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "MessageHandler.h"
#include "Scanner.h"

using namespace std;

int nextInt(SOCKET soc, int& rescode);
string nextStr(SOCKET soc, int& rescode);

Server::Server()
{
	// notice that we step out to the global namespace
	// for the resolution of the function socket
	
	// this server use TCP. that why SOCK_STREAM & IPPROTO_TCP
	// if the server use UDP we will use: SOCK_DGRAM & IPPROTO_UDP
	_serverSocket = ::socket(AF_INET,  SOCK_STREAM,  IPPROTO_TCP); 

	if (_serverSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket");
}

Server::~Server()
{
	try
	{
		// the only use of the destructor should be for freeing 
		// resources that was allocated in the constructor
		::closesocket(_serverSocket);
	}
	catch (...) {}
}

void Server::serve(int port)
{
	
	struct sockaddr_in sa = { 0 };
	
	sa.sin_port = htons(port); // port that server will listen for
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = INADDR_ANY;    // when there are few ip's for the machine. We will use always "INADDR_ANY"

	// again stepping out to the global namespace
	// Connects between the socket and the configuration (port and etc..)
	if (::bind(_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");
	
	// Start listening for incoming requests of clients
	if (::listen(_serverSocket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");
	cout << "Listening on port " << port << endl;

	while (MessageHandler::instance()->_active)
	{
		// the main thread is only accepting clients 
		// and add then to the list of handlers
		accept();
	}
}


void Server::accept()
{
	// this accepts the client and create a specific socket from server to this client
	SOCKET client_socket = ::accept(_serverSocket, NULL, NULL);

	if (client_socket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__);

	if (!MessageHandler::instance()->_active) return;


	// the function that handle the conversation with the client
	thread t(&Server::clientHandler, client_socket);
	t.detach();
}


void Server::clientHandler(SOCKET clientSocket)
{
	int res_code = 1; 
	Scanner scanner(clientSocket);
	while (res_code > 0)
	{
		try
		{
			scanner.wait();
			int code = scanner.nextInt();
			scanner.block();
			unique_lock<mutex> l(MessageHandler::instance()->getQueueLocker());
			MessageHandler::instance()->messageQueue.push(Message(code, &scanner));
			l.unlock();
			if (code == 299)
			{
				break;
			}
		}
		catch (...)
		{
			break;
		}
	}
	cout << "A socket has ended or forced to end." << endl;
	scanner.close();
}


string nextStr(SOCKET soc, int& rescode)
{
	try
	{
		vector<char> chrs;
		int s = 0;
		char *buf = new char();
		while (s < 2)
		{
			rescode = recv(soc, buf, 1, 0);
			if (*buf == '#')
			{
				s++;
			}
			else
			{
				s ^= s;
				chrs.push_back(*buf);
			}
		}
		string strrep(chrs.begin(), chrs.end());
		delete buf;
		return strrep;
	}
	catch (...)
	{
		rescode = -1;
		return string();
	}
}

int nextInt(SOCKET soc, int& rescode)
{
	try
	{
		string strrep = nextStr(soc, rescode);
		int intrep = atoi(strrep.c_str());
		return intrep;
	}
	catch (...)
	{
		rescode = -1;
		return 0;
	}
}
