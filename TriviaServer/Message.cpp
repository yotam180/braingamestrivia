#include "Message.h"



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
