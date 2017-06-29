#pragma once
#include <iostream>
#include <string>
using namespace std;

/*
Used to define a question - with the question string, answers, category, solution and difficulty.
*/
struct Question
{
	Question() {}
	Question(string question, string correct, string incorrect1, string incorrect2, string incorrect3, int category, string difficulty);
	~Question();

	string question;
	int category;
	string difficulty;
	string ans1, ans2, ans3, ans4;
	int correctAnswer;
	long long time;
};

