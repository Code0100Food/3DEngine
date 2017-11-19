#include "TimeManager.h"

// Constructors =================================
TimeManager::TimeManager()
{

}

// Destructors ==================================
TimeManager::~TimeManager()
{

}

// Get Methods ==================================
float TimeManager::GetGameTimeSinceStartup() const
{
	return game_time_since_startup;
}

float TimeManager::GetGameTimeScale() const
{
	return game_time_scale;
}

float TimeManager::GetGameDT()
{
	return game_dt = this->real_time_dt * game_time_scale;
}

// Set Methods ==================================
void TimeManager::SetGameTimeSinceStartup(float val)
{
	game_time_since_startup = val;
}

void TimeManager::SetGameTimeScale(float val)
{
	game_time_scale = val;
}

// Functionality ================================
void TimeManager::AddGameTimeSinceStartup(float val)
{
	game_time_since_startup += val;
}
