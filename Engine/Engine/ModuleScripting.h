#ifndef _MODULE_SCRIPTING_
#define _MODULE_SCRIPTING_

#include "Module.h"

class ScriptManager;
class TextEditor;
class ResourceScript;
class GameObject;
class ComponentScript;
typedef struct _MonoObject MonoObject;
typedef struct _MonoImage MonoImage;
typedef struct _MonoAssemblyName MonoAssemblyName;
typedef struct _MonoString MonoString;
typedef struct _MonoClassField MonoClassField;
enum FIELD_TYPE;

class ModuleScripting : public Module
{
public:

	ModuleScripting(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~ModuleScripting();
	
public:

	bool			Start();
	update_status	Update(float dt);

private:
	MonoAssemblyName*		enviroment_assembly = nullptr;
	MonoImage*				enviroment_image = nullptr;
	ResourceScript*			focused_script_resource = nullptr;
	GameObject*				focused_gameobject = nullptr;
	
	TextEditor*				text_editor = nullptr;
	bool					show_text_editor = false;

	bool					show_script_creation_win = false;
	char					name_buffer[200];
	
	mutable std::string								usable_str;
	std::vector<std::pair<std::string, FIELD_TYPE>> fields_vec;

	const char*				dll_path = nullptr;

	//Current Executing Script
	ComponentScript* active_script;

public:

	//Set Methods -----------
	void SetFocusedScriptResource(const ResourceScript* res);

	//Get Methods -----------
	ResourceScript*		GetFocusedScriptResource()const;
	const char*			GetDLLPath()const;
	MonoAssemblyName*	GetEnviromentAssembly()const;
	MonoImage*			GetEnviromentImage()const;

	//Functionality ---------
	//UI
	void												PlaceFocusedScriptOnEditor();
	void												EnableScripCreationWindow(const GameObject* target = nullptr);
	void												BlitScriptingError();
	void												SetCurrentScript(ComponentScript* script);

	//Mono & Data
	void												LoadAppDomain();
	void												ReloadEngineEnvironment();
	void												ReloadScripts(const ResourceScript* res);
	void												UnLoadAppDomain();
	bool												Compile(const char* path, const char* output);
	MonoAssemblyName*									LoadScriptAssembly(const char* assembly_path);
	MonoObject*											CreateMonoObject(MonoAssemblyName* assembly, const char* class_name, const char* name_space);
	MonoObject*											CreateMonoObjectFromScriptResource(ResourceScript* res);
	bool												ExecuteMethod(MonoObject* script, const char* name);
	std::vector<std::pair<std::string, FIELD_TYPE>>*	GetFieldsNameAndType(MonoObject* script);
	unsigned int										GetFieldValue(MonoObject* script, const char* field_name, void** output_value);
	bool												SetFieldValue(MonoObject* script, const char* field_name, void* input_value);
	bool												SetFieldValue(MonoObject* script, MonoClassField* field_name, void* input_value);
	FIELD_TYPE											StrToFieldType(const char* str)const;
	const char*											FieldTypeToStr(FIELD_TYPE type)const;
	void												AddInternalCall(const char*  cs_path, const void*  method);
	const char *										MonoStringToChar(MonoString* string);

	MonoImage*											LoadMonoImage(MonoAssemblyName* assembly);
	MonoClassField*										GetFieldFromImage(MonoImage* image, const char* name_space, const char* name, const char* field_name);

	//Mono Methods

	//Transform
	static MonoObject*											GetLocalPosition();
	static void													SetLocalPosition(MonoObject* vector);
	static void													RotateFromEulerAngles(MonoObject* e_angles);

	static MonoObject*											GetForwardDirection();
	static MonoObject*											GetRightDirection();

};
#endif // !_MODULE_SCRIPTING_
