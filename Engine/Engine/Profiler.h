#ifndef  _PROFILER_H_
#define  _PROFILER_H_

#include "Module.h"

enum LOOP_STEP
{
	UNDEF_SEP = 0,
	BUILD_STEP,
	AWAKE_STEP,
	INIT_STEP,
	START_STEP,
	PRE_UPDATE_STEP,
	UPDATE_STEP,
	POST_UPDATE_STEP
};

struct Prof_Block
{
	Prof_Block(MODULE_ID _module_id, LOOP_STEP _loop_step, uint64 _time_in_nanoseconds) :module_id(_module_id), loop_step(_loop_step), time_in_nanoseconds(_time_in_nanoseconds) {}

	MODULE_ID	module_id = UNDEF_MODULE;
	LOOP_STEP	loop_step = UNDEF_SEP;
	uint64		time_in_nanoseconds = 0;
};

class Profiler
{
public:

	Profiler();
	~Profiler();

private:

	//Track all the modules timing
	std::vector<Prof_Block>	current_profiled_blocks;

public:

	//Draw 
	void		BlitInfo();

	//Functionality
	void		BlitModuleProfile(MODULE_ID id, bool graph = false);
	void		CallProfBlock(MODULE_ID id, LOOP_STEP step, uint64 time);
	Prof_Block*	GetProfBlock(MODULE_ID id, LOOP_STEP step)const;

};
#endif // ! _PROFILER_H_
