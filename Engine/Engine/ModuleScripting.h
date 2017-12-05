#ifndef _MODULE_SCRIPTING_
#define _MODULE_SCRIPTING_

#include "Module.h"

class ModuleScripting : public Module
{
public:

	ModuleScripting(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~ModuleScripting();
	
private:

public:

};
#endif // !_MODULE_SCRIPTING_
