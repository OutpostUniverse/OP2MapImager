#pragma once

#include <chrono>

class Timer
{
public:
	void startTimer();
	double getElapsedTime();

private:
	std::chrono::high_resolution_clock::time_point startTime;
};
