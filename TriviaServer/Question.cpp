#include "Question.h"

#include <iostream>
#include <random>
#include <algorithm>

Question::Question(string question, string correct, string incorrect1, string incorrect2, string incorrect3, int category, string difficulty)
{
	string incorrects[3] = { incorrect1, incorrect2, incorrect3 };
	std::random_shuffle(incorrects, incorrects + 2);
	correctAnswer = rand() % 4 + 1;
	this->question = question;
	this->category = category;
	this->difficulty = difficulty;
	if (correctAnswer == 1) // GODAMMIT. I was really tired while writing this code. Have merci on me.
	{
		ans1 = correct;
		ans2 = incorrects[0];
	}
	else
	{
		ans1 = incorrects[0];
		if (correctAnswer == 2)
		{
			ans2 = correct;
			ans3 = incorrects[1];
		}
		else
		{
			ans2 = incorrects[1];
			if (correctAnswer == 3)
			{
				ans3 = correct;
				ans4 = incorrects[2];
			}
			else 
			{
				ans3 = incorrects[2];
				ans4 = correct;
			}
		}
	}
}

Question::~Question()
{
}
