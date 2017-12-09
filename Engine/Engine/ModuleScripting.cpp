#include "ModuleScripting.h"

#include "Application.h"
#include "ScriptManager.h"
#include "TextEditor.h"
#include "ModuleInput.h"
#include "ResourceScript.h"
#include "ImporterManager.h"
#include "ResourcesManager.h"
#include "FileSystem.h"
#include "GameObject.h"



ModuleScripting::ModuleScripting(const char * _name, MODULE_ID _id, bool _config_menu, bool _enabled) :Module(_name, _id, _config_menu, _enabled)
{
	script_manager = new ScriptManager();
	text_editor = new TextEditor();
}

ModuleScripting::~ModuleScripting()
{
	RELEASE(script_manager);
	RELEASE(text_editor);
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
		char path[256];
		char output_path[256];

		sprintf(path, "%s\\%s%s",script_manager->lol, LIBRARY_SCRIPTS_FOLDER, "HelloWorld.txt");
		sprintf(output_path, "%s\\%s%s", script_manager->lol, LIBRARY_SCRIPTS_FOLDER, "HelloWorld.dll");

		const char* result = script_manager->Compile("C:/Users/Th_Sola/Documents/GitHub/3DEngine/Engine/Data/Library/Scripts/HelloWorld.txt", "C:/Users/Th_Sola/Documents/GitHub/3DEngine/Engine/Data/Library/Scripts/HelloWorld.dll");

		if (result)
		{
			LOG(result);
		}
		else
		{
			LOG("Compilation Success");
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
			char* str = new char[13];
			str = "Empty Script\0";
			new_script->SetBuffer(str);
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
