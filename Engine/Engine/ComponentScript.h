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

public:

	bool Start();

private:

	ResourceScript*				resource_script = nullptr;
	
	MonoObject*					script_object = nullptr;

	std::vector<ScriptField>	fields;

public:

	//Set Methods -----------
	void SetResourceScript(ResourceScript* script);
	void SetMonoObject(const MonoObject* obj);

	//Get Methods -----------
	MonoObject*		GetMonoObject()const;
	ResourceScript* GetResourceScript()const;

	//Functionality ---------
	//Fields Methods
	void UpdateFieldsFromResource();
	void SendFieldsValuesToScript();
	//UI Methods
	void BlitComponentInspector();
	//Save/Load Methods -----
	bool Save(Serializer& array_root)const;
	bool Load(Serializer& data, std::vector<std::pair<Component*, uint>>& links);

};
#endif // !_COMPONENT_SCRIPT_H_
