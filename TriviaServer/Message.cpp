#include "Message.h"


/*
Constructs a new message with a type and a Scanner.
Parameters:
	int type - message type.
	Scanner* soc - the user's socket scanner.
*/
Message::Message(int type, Scanner* soc)
{
	this->type = type;
	this->sender = soc;
}


Message::~Message()
{
}

int Message::getType()
{
	return type;
}

Scanner * Message::getScanner()
{
	return sender;
}
