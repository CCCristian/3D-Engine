#pragma once

#include <random>
#include <chrono>
//#include <iostream>
//#include <fstream>

struct TimeManager
{
private:
	static std::chrono::steady_clock::time_point inceput;
	static std::chrono::steady_clock ceas;
	static std::chrono::steady_clock::time_point lastUpdate;

public:
	static inline void init()
	{
		lastUpdate = inceput = std::chrono::steady_clock::now();
		srand((unsigned)inceput.time_since_epoch().count());
	}
	static inline void update()
	{
		lastUpdate = std::chrono::steady_clock::now();
		srand((unsigned)inceput.time_since_epoch().count());
	}
	static inline long long elapsedMilliseconds()	// Timpul trecut de la ultimul frame (in milisecunde)
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(ceas.now() - inceput).count();
	}
	static inline long long deltaMilliseconds()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastUpdate).count();
	}
};

extern std::minstd_rand random;
extern std::uniform_real_distribution<> distr;
float random01();
std::string getDirectory(std::string &filePath);
