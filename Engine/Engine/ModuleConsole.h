#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "Module.h"
#include "BasicConsole.h"

class ModuleConsole : public Module
{
public:

	ModuleConsole(bool start_enabled = true);
	~ModuleConsole();

public:

	bool			Start()final;
	update_status	Update(float dt)final;
	void			BlitConfigInfo()final;

private:

	/*Console Data*/
	std::string console_buffer;
	bool		open = false;

public:

	BasicConsole console;

public:
	
	// Functionality --------
	void SwapConsoleState();
	void AddLabel(const char* str);

};
#endif // !_CONSOLE_H_
