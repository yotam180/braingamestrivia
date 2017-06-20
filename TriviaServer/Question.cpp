#include "Question.h"

#include <iostream>
#include <random>
#include <algorithm>

Question::Question(string question, string correct, string incorrect1, string incorrect2, string incorrect3, int category, string difficulty)
{
	vector<string> incorrects = { incorrect1, incorrect2, incorrect3 };
	correctAnswer = rand() % 4 + 1;
	int num;
	this->question = question;
	this->category = category;
	this->difficulty = difficulty;
	switch (correctAnswer)
	{
	case 1:
		this->ans1 = correct;
		num = rand() % 3;
		this->ans2 = incorrects[num];
		incorrects.erase(incorrects.begin() + num);
		num = rand() % 2;
		this->ans3 = incorrects[num];
		this->ans4 = incorrects[1 - num];
		break;
	case 2:
		this->ans2 = correct;
		num = rand() % 3;
		this->ans1 = incorrects[num];
		incorrects.erase(incorrects.begin() + num);
		num = rand() % 2;
		this->ans3 = incorrects[num];
		this->ans4 = incorrects[1 - num];
		break;
	case 3:
		this->ans3 = correct;
		num = rand() % 3;
		this->ans2 = incorrects[num];
		incorrects.erase(incorrects.begin() + num);
		num = rand() % 2;
		this->ans1 = incorrects[num];
		this->ans4 = incorrects[1 - num];
		break;
	case 4:
		this->ans4 = correct;
		num = rand() % 3;
		this->ans1 = incorrects[num];
		incorrects.erase(incorrects.begin() + num);
		num = rand() % 2;
		this->ans2 = incorrects[num];
		this->ans3 = incorrects[1 - num];
		break;
	}
	/*if (correctAnswer == 1) // GODAMMIT. I was really tired while writing this code. Have merci on me.
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
	}*/
}

Question::~Question()
{
}
