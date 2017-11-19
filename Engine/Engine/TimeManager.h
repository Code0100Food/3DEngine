#ifndef _TIME_MANAGER_
#define _TIME_MANAGER_

#include "Module.h"
#include "Timer.h"

class TimeManager
{

	friend class Application;
	friend class Scene;

public:

	TimeManager();
	~TimeManager();

private:

	//App timing
	Timer	ms_timer;
	uint64	frame_count = 0;
	Timer	real_startup_time;
	float	real_time_since_startup = 0;
	Timer	frame_time;
	Timer	last_sec_frame_time;
	uint32	last_sec_frame_count = 0;
	uint32	prev_last_sec_frame_count = 0;
	float	real_time_dt = 0.0f;
	int		capped_ms = -1;
	int		max_fps = 0;

	//Game timing
	float	game_time_since_startup = 0;
	float	game_time_scale = 0;
	float	game_dt = 0;

public:

	//Get Methods -----------
	float GetGameTimeSinceStartup()const;
	float GetGameTimeScale()const;
	float GetGameDT();

	//Set Methods -----------
	void SetGameTimeSinceStartup(float val);
	void SetGameTimeScale(float val);
	
	//Functionality ---------
	void AddGameTimeSinceStartup(float val);

};
#endif // !_TIME_MANAGER_
