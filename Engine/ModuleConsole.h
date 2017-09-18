#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include ""
class ModuleConsole : public Module
{
public:

	ModuleConsole(Application* parent, bool start_enabled = true);
	~ModuleConsole();

public:

	bool Start()final;
	update_state Update(float dt)final;

};
#endif // !_CONSOLE_H_
