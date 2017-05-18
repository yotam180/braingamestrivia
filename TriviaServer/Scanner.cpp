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

void Scanner::writeStr(string val)
{
	try
	{
		send(soc, val.c_str(), val.size(), 0);
		send(soc, "##", 2, 0);
	}
	catch (...)
	{
		cout << "Error while writing to client" << endl;
	}
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
