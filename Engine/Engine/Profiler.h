#ifndef  _PROFILER_H_
#define  _PROFILER_H_

#include "Module.h"

class Profiler : public Module
{
public:

	Profiler(bool start_enabled = true);
	~Profiler();

public:

	bool Awake(const JSON_Object* data_root);
	/*Don't need update*/

public:

	/*Track & Cap the milliseconds*/

};
#endif // ! _PROFILER_H_
