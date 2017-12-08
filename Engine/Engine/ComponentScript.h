#ifndef _COMPONENT_SCRIPT_H_
#define _COMPONENT_SCRIPT_H_

#include "Component.h"

class ResourceScript;

class ComponentScript : public Component
{
public:

	ComponentScript();
	ComponentScript(const ComponentScript& cpy);
	~ComponentScript();

private:

	ResourceScript*	resource_script = nullptr;

public:

	//Set Methods -----------
	void SetResourceScript(ResourceScript* script);

	//Get Methods -----------
	

	//UI Methods
	void BlitComponentInspector();

};
#endif // !_COMPONENT_SCRIPT_H_
