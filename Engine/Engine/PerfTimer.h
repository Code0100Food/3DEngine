#ifndef __PERFTIMER_H__
#define __PERFTIMER_H__

#include "Globals.h"

class PerfTimer
{
public:

	// Constructor
	PerfTimer();

public:

	void	Start();
	double	ReadMs() const;
	uint64	ReadTicks() const;

private:

	uint64			started_at = 0;
	static uint64	frequency;

};

#endif //__PERFTIMER_H__