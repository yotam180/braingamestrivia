#include "Scanner.h"
#include <vector>
#include <iostream>
using namespace std;

Scanner::Scanner(SOCKET soc)
{
	this->soc = soc;
	blocked = false;
}

Scanner::~Scanner()
{
}

int Scanner::nextInt()
{
	string strrep = nextStr();
	int intrep = atoi(strrep.c_str());
	return intrep;
}

string Scanner::nextStr()
{
	vector<char> chrs;
	int s = 0;
	char *buf = new char();
	while (s < 2)
	{
		recv(soc, buf, 1, 0);
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

void Scanner::close()
{
	closesocket(soc);
}

void Scanner::writeInt(int val)
{
	writeStr(std::to_string(val));
}

void SEND(SOCKET soc, const char arr[], int size)
{
	__try
	{
		send(soc, arr, size, 0);
	}
	__except (
		GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION
		? EXCEPTION_EXECUTE_HANDLER
		: EXCEPTION_CONTINUE_SEARCH) {
		std::cerr << "OMG!\n";
	}
}

void Scanner::writeStr(string val)
{
	SEND(soc, val.c_str(), val.size());
	SEND(soc, "##", 2);	
}

void Scanner::block()
{
	blocked = true;
}

void Scanner::release()
{
	blocked = false;
}

void Scanner::wait()
{
	while (blocked) Sleep(1);
}
