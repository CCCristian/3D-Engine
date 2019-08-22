#pragma once

struct TimeManager
{
private:
	static std::chrono::steady_clock::time_point inceput;
	static std::chrono::steady_clock ceas;
	static std::chrono::steady_clock::time_point lastUpdate;

public:
	static void init();
	static void update();
	static long long elapsedMilliseconds();	// Timpul trecut de la ultimul frame (in milisecunde)
	static long long deltaMilliseconds();
};

extern std::minstd_rand random;
extern std::uniform_real_distribution<> distr;
float random01();
std::string getDirectory(std::string &filePath);
