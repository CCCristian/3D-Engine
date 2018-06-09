#include "Utils.h"

std::minstd_rand random((unsigned int)std::chrono::steady_clock::now().time_since_epoch().count());
std::uniform_real_distribution<> distr(0.0f, 1.0f);
std::chrono::steady_clock::time_point TimeManager::inceput;
std::chrono::steady_clock TimeManager::ceas;
std::chrono::steady_clock::time_point TimeManager::lastUpdate;

float random01()
{
	return (float)distr(random);
}

std::string getDirectory(std::string &filePath)
{
	return filePath.substr(0, filePath.find_last_of("/\\") + 1);
}
