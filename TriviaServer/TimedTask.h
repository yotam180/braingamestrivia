#pragma once

class TimedTask
{
public:
	TimedTask(long long timeout, int type, void* param);
	~TimedTask();

	long long timeout;
	int type;
	void* param;

	static const int
		START_GAME = 1,
		END_QUESTION = 2,
		END_ANSWERS = 3,
		END_PODIUM = 4,
		END_GAME = 5;
};

