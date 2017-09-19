#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "Module.h"
#include "BasicConsole.h"

class ModuleConsole : public Module
{
public:

	ModuleConsole(Application* parent, bool start_enabled = true);
	~ModuleConsole();

public:

	bool			Start()final;
	update_status	Update(float dt)final;

private:

	/*Console Data*/
	std::string console_buffer;
	bool		open = true;

public:

	BasicConsole console;

public:
	
	// Functionality --------
	void SwapConsoleState();
	
};
#endif // !_CONSOLE_H_
