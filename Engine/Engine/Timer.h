#ifndef __TIMER_H__
#define __TIMER_H__

#include "Globals.h"
#include "SDL/include/SDL.h"


class Timer
{
public:

	Timer();
	~Timer();

private:

	bool	running = false;
	Uint32	started_at = 0;
	Uint32	stopped_at = 0;

public:


	void	Start();
	void	Stop();
	Uint32	Read() const;
	float	ReadSec() const;

};

#endif //__TIMER_H__