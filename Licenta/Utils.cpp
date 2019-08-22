#include "pch.h"
#include "Utils.h"

std::minstd_rand random((unsigned int)std::chrono::steady_clock::now().time_since_epoch().count());
std::uniform_real_distribution<> distr(0.0f, 1.0f);
std::chrono::steady_clock::time_point TimeManager::inceput;
std::chrono::steady_clock TimeManager::ceas;
std::chrono::steady_clock::time_point TimeManager::lastUpdate;

void TimeManager::init()
{
	lastUpdate = inceput = std::chrono::steady_clock::now();
	srand((unsigned)inceput.time_since_epoch().count());
}
void TimeManager::update()
{
	lastUpdate = std::chrono::steady_clock::now();
	srand((unsigned)inceput.time_since_epoch().count());
}
long long TimeManager::elapsedMilliseconds()	// Timpul trecut de la ultimul frame (in milisecunde)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(ceas.now() - inceput).count();
}
long long TimeManager::deltaMilliseconds()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastUpdate).count();
}

float random01()
{
	return (float)distr(random);
}

std::string getDirectory(std::string &filePath)
{
	return filePath.substr(0, filePath.find_last_of("/\\") + 1);
}
