#include "Timer.h"

using namespace std;
using namespace std::chrono;

void Timer::startTimer()
{
	startTime = high_resolution_clock::now();
}

double Timer::getElapsedTime()
{
	high_resolution_clock::time_point currentTime = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(currentTime - startTime);
	
	return time_span.count();
}