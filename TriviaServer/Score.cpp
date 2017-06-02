#include "Score.h"

int Score::GetScore()
{
	return score;
}

void Score::SetScore(int s)
{
	this->score = s;
}

Score::Score()
{
	this->score = 0;
}
