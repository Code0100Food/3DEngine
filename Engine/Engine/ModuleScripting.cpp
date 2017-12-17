#include "ModuleScripting.h"

#include "Application.h"
#include "TextEditor.h"
#include "ModuleInput.h"
#include "ResourceScript.h"
#include "ImporterManager.h"
#include "ResourcesManager.h"
#include "FileSystem.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "TimeManager.h"

#include "ComponentScript.h"

#include "Mono_Scripting/test.h"
#pragma comment(lib, "Engine/Mono_Scripting/MonoScripting.lib")

ModuleScripting::ModuleScripting(const char * _name, MODULE_ID _id, bool _config_menu, bool _enabled) :Module(_name, _id, _config_menu, _enabled)
{
	MonoScripting::InitMono();
	
	MonoScripting::LoadAppDomain();

	dll_path = MonoScripting::GetDLLPath();

	text_editor = new TextEditor();

	char environment_path[250];
	sprintf(environment_path, "%s/%s%s", dll_path, SCRIPTING_FOLDER, "FiestaEngineEnviroment.dll");
	
	enviroment_assembly = MonoScripting::LoadScriptAssembly(environment_path);

	if (enviroment_assembly != nullptr)
	{
		enviroment_image = MonoScripting::LoadMonoImage(enviroment_assembly);
		LOG("Fiesta Engine Environment Correctly Loaded!");
	}
	else
	{
		LOG("[error] Error on Environment Load!");
	}
}

ModuleScripting::~ModuleScripting()
{
	MonoScripting::CleanUpMono();

	RELEASE(text_editor);
}

bool ModuleScripting::Start()
{
	active_script = nullptr;

	//Add Internal all methods

	//Transform
	AddInternalCall("FiestaEngine.Transform::GetPosition", GetLocalPosition);
	AddInternalCall("FiestaEngine.Transform::SetPosition", SetLocalPosition);
	AddInternalCall("FiestaEngine.Transform::ToEulerAngles", RotateFromEulerAngles);

	AddInternalCall("FiestaEngine.Transform::GetForward", GetForwardDirection);
	AddInternalCall("FiestaEngine.Transform::GetRight", GetRightDirection);

	//Time
	AddInternalCall("FiestaEngine.Time::GetDeltaTime", App->time_manager->GetScriptingDT);

	return true;
}

update_status ModuleScripting::Update(float dt)
{
	//Scripting console inputs
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_STATE::KEY_DOWN)
	{
		show_text_editor = !show_text_editor;
		if (!show_text_editor)
		{
			text_editor->SetSelection({ 0,0 }, { 999,999 });
			text_editor->Delete();
			focused_script_resource = nullptr;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_STATE::KEY_DOWN)
	{
		/*char path[256];
		char output_path[256];

		sprintf(path, "%s\\%s%s",dll_path, SCRIPTING_FOLDER, "FiestaEngineEnviroment.txt");
		sprintf(output_path, "%s\\%s%s", dll_path, SCRIPTING_FOLDER, "FiestaEngineEnviroment.dll");

		if (!Compile(path, output_path))
		{
			BlitScriptingError();
		}
		else
		{
			App->fs->GetFileNameFromPath(output_path, &usable_str);
			LOG("Compilation Success: %s", usable_str.c_str());
		}*/

		//Fiesta engine system
		char path1[256];
		char output_path1[256];
		
		sprintf(path1, "%s\\%s%s", dll_path, LIBRARY_SCRIPTS_FOLDER, "HelloWorld.txt");
		sprintf(output_path1, "%s\\%s%s", dll_path, LIBRARY_SCRIPTS_FOLDER, "HelloWorld.dll");		

		if (!Compile(path1, output_path1))
		{
			BlitScriptingError();
		}
		else
		{
			App->fs->GetFileNameFromPath(output_path1, &usable_str);
			LOG("Compilation Success: %s", usable_str.c_str());
		}

	}

	if (show_text_editor)
	{
		//Blit text editor
		ImGui::Begin("Script Console", &show_text_editor);
		if (ImGui::Button("Load Last"))
		{
			PlaceFocusedScriptOnEditor();
		}
		ImGui::SameLine();
		if (ImGui::Button("Save"))
		{
			if (focused_script_resource != nullptr)
			{
				usable_str = text_editor->GetText();
				uint size = strlen(usable_str.c_str());
				char* real_str = new char[size + 1];
				memcpy(real_str, usable_str.c_str(), size);
				real_str[size] = '\0';
				focused_script_resource->SetBuffer(real_str);
				App->importer->script_importer.NewImport(focused_script_resource);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("New Script"))
		{
			EnableScripCreationWindow();
		}

		text_editor->Render("Script Console", ImVec2(600, 400));
		ImGui::End();

		//Line Jump
		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			text_editor->InsertText("\n");
		}
		//Tab
		else if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN)
		{
			text_editor->InsertText("    ");
		}
		//Ctrl C
		else if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
		{
			if (text_editor->HasSelection())text_editor->Copy();
		}
		//Ctrl V
		else if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
		{
			text_editor->Paste();
		}
		//Ctrl X
		else if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)
		{
			if(text_editor->HasSelection())text_editor->Cut();
		}
		//Ctrl Z
		else if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
		{
			if(text_editor->CanUndo())text_editor->Undo();
		}
		//Ctrl Y
		else if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN)
		{
			if(text_editor->CanRedo())text_editor->Redo();
		}
	}
	
	if (show_script_creation_win)
	{
		ImGui::Begin("Script Console", &show_script_creation_win);
		
		ImGui::InputText("Name", name_buffer, 200);

		if (ImGui::Button("Create"))
		{
			ResourceScript* new_script = (ResourceScript*)App->res_manager->CreateResource(RESOURCE_TYPE::SCRIPT_RESOURCE);
			
			/*char* str = new char[14];
			str = "Empty Script\0";
			new_script->SetBuffer(str);*/
			
			char name[210];
			sprintf(name, "%s.txt", name_buffer);
			new_script->SetOwnFile(name);
			
			char original_name[250];
			sprintf(original_name, "%s\\%s", App->fs->GetAssetsFolder()->GetPath(), name);
			new_script->SetOriginalFile(original_name);
			App->importer->script_importer.NewImport(new_script);

			if (focused_gameobject != nullptr)
			{
				ComponentScript* scr_cmp = (ComponentScript*)focused_gameobject->CreateComponent(COMPONENT_TYPE::COMP_SCRIPT);
				scr_cmp->SetResourceScript(new_script);

				focused_gameobject = nullptr;
			}

			focused_script_resource = new_script;
			PlaceFocusedScriptOnEditor();

			show_script_creation_win = false;
		}

		ImGui::End();
	}

	return update_status::UPDATE_CONTINUE;
}

void ModuleScripting::SetFocusedScriptResource(const ResourceScript * res)
{
	focused_script_resource = (ResourceScript*)res;
}

ResourceScript * ModuleScripting::GetFocusedScriptResource() const
{
	return focused_script_resource;
}

const char * ModuleScripting::GetDLLPath() const
{
	return MonoScripting::GetDLLPath();
}

MonoAssemblyName * ModuleScripting::GetEnviromentAssembly() const
{
	return enviroment_assembly;
}

MonoImage * ModuleScripting::GetEnviromentImage() const
{
	return enviroment_image;
}

void ModuleScripting::PlaceFocusedScriptOnEditor()
{
	if (focused_script_resource == nullptr)return;

	//Open the scripting console
	if (!show_text_editor)show_text_editor = true;
	
	//Delete the current text
	text_editor->SetSelection({ 0,0 }, { 999,999 });
	text_editor->Delete();
	
	//Insert the script text
	text_editor->InsertText(focused_script_resource->GetBuffer());
}

void ModuleScripting::EnableScripCreationWindow(const GameObject* target)
{
	focused_gameobject = (GameObject*)target;

	memset(name_buffer, '\0', 200);
	show_script_creation_win = true;
}

void ModuleScripting::BlitScriptingError()
{
	const char* str = MonoScripting::GetLastError();
	if (str != NULL)
	{
		LOG("%s", str);
	}
}

void ModuleScripting::SetCurrentScript(ComponentScript * script)
{
	active_script = script;
}

void ModuleScripting::LoadAppDomain()
{
	MonoScripting::LoadAppDomain();
}

void ModuleScripting::ReloadEngineEnvironment()
{
	char environment_path[250];
	sprintf(environment_path, "%s/%s%s", dll_path, SCRIPTING_FOLDER, "FiestaEngineEnviroment.dll");

	enviroment_assembly = MonoScripting::LoadScriptAssembly(environment_path);

	if (enviroment_assembly != nullptr)
	{
		enviroment_image = MonoScripting::LoadMonoImage(enviroment_assembly);
		LOG("Fiesta Engine Environment Correctly Loaded!");
	}
	else
	{
		LOG("[error] Error on Environment Load!");
	}

}

void ModuleScripting::ReloadScripts(const ResourceScript * res)
{
	//Collect all the script resources
	vector<Resource*>* scripts_res_vec = App->res_manager->GetResourcesVectorByType(RESOURCE_TYPE::SCRIPT_RESOURCE);
	//Collect all the script components
	std::vector<Component*>* components_vec = App->scene->FindComponentsByType(COMPONENT_TYPE::COMP_SCRIPT);

	uint size = scripts_res_vec->size();
	for (uint k = 0; k < size; k++)
	{
		if (scripts_res_vec->at(k) != res)
		{
			//Reload assembly and objects for the components!
			char out_path[250];
			sprintf(out_path, "%s/%s/%s", GetDLLPath(), LIBRARY_SCRIPTS_FOLDER, scripts_res_vec->at(k)->GetOwnFile());

			MonoAssemblyName* asm_ = LoadScriptAssembly(out_path);
			if (asm_ == nullptr)BlitScriptingError();
			else
			{
				//Reload resource assembly
				((ResourceScript*)scripts_res_vec->at(k))->SetAssembly(asm_);
				
				//Iterate all the components
				uint size = components_vec->size();
				for (uint g = 0; g < size; g++)
				{
					//Only the comps related with the actual resource are updated
					if (((ComponentScript*)components_vec->at(g))->GetResourceScript()->GetID() == scripts_res_vec->at(k)->GetID())
					{
						//Create the objects
						App->fs->GetUnformatedFileNameFromPath(scripts_res_vec->at(k)->GetOwnFile(), &usable_str);
						MonoObject* obj = App->scripting->CreateMonoObject(asm_, usable_str.c_str(), "");
						if (obj == nullptr)BlitScriptingError();
						else
						{
							((ComponentScript*)components_vec->at(g))->SetMonoObject(obj);
						}
					}
				}
			}
		}
	}
}

void ModuleScripting::UnLoadAppDomain()
{
	MonoScripting::UnLoadAppDomain();
}

bool ModuleScripting::Compile(const char * path, const char * output)
{
	return MonoScripting::CompileFile(path, output);
}

MonoAssemblyName * ModuleScripting::LoadScriptAssembly(const char * assembly_path)
{
	return MonoScripting::LoadScriptAssembly(assembly_path);
}

MonoObject * ModuleScripting::CreateMonoObject(MonoAssemblyName * assembly, const char * class_name, const char * name_space)
{
	return MonoScripting::CreateMonoObject(assembly, class_name, name_space);
}

MonoObject * ModuleScripting::CreateMonoObjectFromScriptResource(ResourceScript * res)
{
	if (res->GetAssembly() == nullptr)
	{
		return nullptr;
	}

	App->fs->GetUnformatedFileNameFromPath(res->GetOwnFile(), &usable_str);

	return MonoScripting::CreateMonoObject(res->GetAssembly(), usable_str.c_str(), "");
}

bool ModuleScripting::ExecuteMethod(MonoObject * script, const char * name)
{
	return MonoScripting::ExecuteMethod(script, name);
}

std::vector<std::pair<std::string, FIELD_TYPE>>* ModuleScripting::GetFieldsNameAndType(MonoObject * script)
{
	//Reset fields
	fields_vec.clear();

	//Collect all the fields str data
	void* iterator = nullptr;
	do {

		const char* field_str = MonoScripting::GetFieldsNameAndType(script, &iterator);
		if (field_str != NULL)
		{
			char cpy_fld_str[200];
			strcpy(cpy_fld_str, field_str);
			usable_str = cpy_fld_str;

			const char* str = strtok((char*)usable_str.c_str(), "/");
			const char* t_str = strtok(NULL, "/");

			std::pair<std::string, FIELD_TYPE> pair;
			pair.first = str;
			pair.second = StrToFieldType(t_str);

			fields_vec.push_back(pair);
		}
		
	} while (iterator != nullptr);


	return &fields_vec;
}

unsigned int ModuleScripting::GetFieldValue(MonoObject * script, const char * field_name, void** output_value)
{
	return MonoScripting::GetFieldValue(script, field_name, output_value);
}

bool ModuleScripting::SetFieldValue(MonoObject * script, const char * field_name, void * input_value)
{
	return MonoScripting::SetFieldValue(script, field_name, input_value);
}

bool ModuleScripting::SetFieldValue(MonoObject * script, MonoClassField* field, void * input_value)
{
	MonoScripting::SetFieldValue(script, field, input_value);
	return true;
}

FIELD_TYPE ModuleScripting::StrToFieldType(const char * str) const
{
	if (strcmp(str, "System.Int8") == 0)	return FIELD_TYPE::INT8_FIELD;
	if (strcmp(str, "System.Int16") == 0)	return FIELD_TYPE::INT16_FIELD;
	if (strcmp(str, "System.Int32") == 0)	return FIELD_TYPE::INT32_FIELD;
	if (strcmp(str, "System.Int64") == 0)	return FIELD_TYPE::INT64_FIELD;
	if (strcmp(str, "System.UInt8") == 0)	return FIELD_TYPE::UINT8_FIELD;
	if (strcmp(str, "System.UInt16") == 0)	return FIELD_TYPE::UINT16_FIELD;
	if (strcmp(str, "System.UInt32") == 0)	return FIELD_TYPE::UINT32_FIELD;
	if (strcmp(str, "System.UInt64") == 0)	return FIELD_TYPE::UINT64_FIELD;
	if (strcmp(str, "System.Single") == 0)	return FIELD_TYPE::FLOAT_FIELD;
	if (strcmp(str, "System.Boolean") == 0)	return FIELD_TYPE::BOOL_FIELD;
	if (strcmp(str, "FiestaEngine.GameObject") == 0)	return FIELD_TYPE::OBJECT_FIELD;
	return FIELD_TYPE::UNDEF_FIELD_TYPE;
}

const char * ModuleScripting::FieldTypeToStr(FIELD_TYPE type) const
{
	switch (type)
	{
	case INT8_FIELD:	return "System.Int8";				break;
	case INT16_FIELD:	return "System.Int16";				break;
	case INT32_FIELD:	return "System.Int32";				break;
	case INT64_FIELD:	return "System.Int64";				break;
	case UINT8_FIELD:	return "System.UInt8";				break;
	case UINT16_FIELD:	return "System.UInt16";				break; 
	case UINT32_FIELD:	return "System.UInt32";				break; 
	case UINT64_FIELD:	return "System.UInt64";				break; 
	case FLOAT_FIELD:	return "System.Single";				break;
	case BOOL_FIELD:	return "System.Boolean";			break;
	case OBJECT_FIELD:	return "FiestaEngine.GameObject";	break;
	}
	return "undef_field_type";
}

void ModuleScripting::AddInternalCall(const char*  cs_path, const void*  method)
{
	MonoScripting::AddMethodInternalCall(cs_path, method);
}

const char * ModuleScripting::MonoStringToChar(MonoString* string)
{
	return MonoScripting::MonoStringToConstChar(string);
}

MonoImage * ModuleScripting::LoadMonoImage(MonoAssemblyName* assembly)
{
	return MonoScripting::LoadMonoImage(assembly);
}

MonoClassField* ModuleScripting::GetFieldFromImage(MonoImage * image, const char * name_space, const char * name, const char * field_name)
{
	return MonoScripting::GetFieldValue(image, name_space, name, field_name);
}

//MONO METHODS

//Transform
MonoObject* ModuleScripting::GetLocalPosition()
{
	return App->scripting->active_script->GetLocalPosition();
}

void ModuleScripting::SetLocalPosition(MonoObject* vector)
{
	App->scripting->active_script->SetPosition(vector);
}

void ModuleScripting::RotateFromEulerAngles(MonoObject* e_angles)
{
	App->scripting->active_script->RotateFromEulerAngles(e_angles);
}

MonoObject * ModuleScripting::GetForwardDirection()
{
	return App->scripting->active_script->GetForwardDirection();
}

MonoObject * ModuleScripting::GetRightDirection()
{
	return App->scripting->active_script->GetRightDirection();
}
