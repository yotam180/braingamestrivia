#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <mutex>

using namespace std;

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

