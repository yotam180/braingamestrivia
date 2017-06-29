#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <mutex>

using namespace std;

/*
A wrapper class for the native SOCKET commands, able to send and receive values to and from the socket,
thread safe and very simple to use.
*/
class Scanner
{
public:
	Scanner(SOCKET soc);
	~Scanner();
	int nextInt();
	string nextStr();
	void close();
	void writeInt(int val);
	void writeStr(string val);
	void block();
	void release();
	void wait();
private:
	SOCKET soc;
	bool blocked;
};

