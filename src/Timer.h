#pragma once

#include <chrono>

class Timer
{
public:
	void StartTimer();
	double GetElapsedTime();

private:
	std::chrono::high_resolution_clock::time_point startTime;
};
