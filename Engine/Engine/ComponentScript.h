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
	bool Update(float dt);

	bool ScriptUpdate();

private:

	ResourceScript*				resource_script = nullptr;
	
	MonoObject*					script_object = nullptr;
	
	GameObject*					target_object = nullptr;

	std::vector<ScriptField>	fields;

public:

	//Set Methods -----------
	void SetResourceScript(ResourceScript* script);
	void SetMonoObject(const MonoObject* obj);
	void SetTargetObject(const GameObject* target);

	//Get Methods -----------
	MonoObject*		GetMonoObject()const;
	ResourceScript* GetResourceScript()const;
	GameObject*		GetTargetObject()const;

	//Functionality ---------
	//Fields Methods
	void UpdateFieldsFromResource();
	void SendFieldsValuesToScript();
	//UI Methods
	void BlitComponentInspector();
	//Save/Load Methods -----
	bool Save(Serializer& array_root)const;
	bool Load(Serializer& data, std::vector<std::pair<Component*, uint>>& links);

	//MONO METHODS

	//Transform
	MonoObject* ComponentScript::GetLocalPosition();
	void ComponentScript::SetPosition(MonoObject* vector);

};
#endif // !_COMPONENT_SCRIPT_H_
