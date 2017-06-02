#include "TimedTask.h"



TimedTask::TimedTask(long long timeout, int type, void* param)
{
	this->timeout = timeout;
	this->type = type;
	this->param = param;
}


TimedTask::~TimedTask()
{
}
