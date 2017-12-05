#include "ModuleScripting.h"

#include "Application.h"
#include "ScriptManager.h"
#include "TextEditor.h"
#include "ModuleInput.h"

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
	}

	if (show_text_editor)
	{
		//Blit text editor
		ImGui::Begin("Script Console");
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
	
	return update_status::UPDATE_CONTINUE;
}
