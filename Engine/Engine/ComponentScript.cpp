#include "ComponentScript.h"

#include "ResourceScript.h"
#include "Application.h"
#include "ModuleScripting.h"

// Constructors =================================
ComponentScript::ComponentScript() :Component(COMP_SCRIPT)
{

}

ComponentScript::ComponentScript(const ComponentScript & cpy)
{

}

// Destructors ==================================
ComponentScript::~ComponentScript()
{
	if (resource_script != nullptr)resource_script->RestReference();
}

void ComponentScript::SetResourceScript(ResourceScript * script)
{
	resource_script = (ResourceScript*)script;
	script->AddReference();
}

// Functionality ================================
void ComponentScript::BlitComponentInspector()
{
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Script");

	ImGui::Text("%s", resource_script->GetOwnFile());
	ImGui::SameLine();
	
	if (ImGui::Button("Edit"))
	{
		App->scripting->SetFocusedScriptResource(resource_script);
		App->scripting->PlaceFocusedScriptOnEditor();
	}
}
