#ifndef _COMPONENT_SCRIPT_H_
#define _COMPONENT_SCRIPT_H_

#include "Component.h"
#include "ResourceScript.h"

class ComponentScript : public Component
{
public:

	ComponentScript();
	ComponentScript(const ComponentScript& cpy);
	~ComponentScript();

private:

	ResourceScript*				resource_script = nullptr;
	
	MonoObject*					script_object = nullptr;

	std::vector<ScriptField>	fields;

public:

	//Set Methods -----------
	void SetResourceScript(ResourceScript* script);
	void SetMonoObject(const MonoObject* obj);

	//Get Methods -----------
	MonoObject*	GetMonoObject()const;

	//UI Methods
	void BlitComponentInspector();

};
#endif // !_COMPONENT_SCRIPT_H_
