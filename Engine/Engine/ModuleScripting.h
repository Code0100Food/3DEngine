#ifndef _MODULE_SCRIPTING_
#define _MODULE_SCRIPTING_

#include "Module.h"

class ScriptManager;
class TextEditor;
class ResourceScript;
class GameObject;

class ModuleScripting : public Module
{
public:

	ModuleScripting(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~ModuleScripting();
	
public:

	update_status Update(float dt);

private:

	ResourceScript*			focused_script_resource = nullptr;
	GameObject*				focused_gameobject = nullptr;

	TextEditor*				text_editor = nullptr;
	bool					show_text_editor = false;

	bool					show_script_creation_win = false;
	char					name_buffer[200];
	
	mutable std::string		usable_str;
	
	const char*				dll_path = nullptr;

public:

	//Set Methods -----------
	void SetFocusedScriptResource(const ResourceScript* res);

	//Get Methods -----------
	ResourceScript* GetFocusedScriptResource()const;
	const char*		GetDLLPath()const;

	//Functionality ---------
	void		PlaceFocusedScriptOnEditor();
	void		EnableScripCreationWindow(const GameObject* target = nullptr);
	const char* Compile(const char* path, const char* output);

};
#endif // !_MODULE_SCRIPTING_
