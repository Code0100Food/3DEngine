#ifndef _MODULE_SCRIPTING_
#define _MODULE_SCRIPTING_

#include "Module.h"

class ScriptManager;
class TextEditor;
class ResourceScript;

class ModuleScripting : public Module
{
public:

	ModuleScripting(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~ModuleScripting();
	
public:

	update_status Update(float dt);

private:

	ScriptManager*	script_manager = nullptr;
	
	ResourceScript* focused_script_resource = nullptr;

	TextEditor*		text_editor = nullptr;
	bool			show_text_editor = false;

public:

	//Set Methods -----------
	void SetFocusedScriptResource(const ResourceScript* res);

	//Get Methods -----------
	ResourceScript* GetFocusedScriptResource()const;

	//Functionality ---------
	void PlaceFocusedScriptOnEditor();

};
#endif // !_MODULE_SCRIPTING_
