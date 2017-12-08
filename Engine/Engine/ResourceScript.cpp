#include "ResourceScript.h"

#include "imgui/imgui.h"
#include "Application.h"
#include "ModuleScripting.h"

ResourceScript::ResourceScript():Resource(SCRIPT_RESOURCE)
{

}

ResourceScript::~ResourceScript()
{

}

void ResourceScript::SetBuffer(const char * bf)
{
	if (buffer != nullptr)RELEASE_ARRAY(buffer);
	buffer = (char*)bf;
}

char * ResourceScript::GetBuffer() const
{
	return buffer;
}

void ResourceScript::BlitUI() const
{

}

void ResourceScript::BlitComplexUI()
{
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%s", ResourceTypeToStr(type));
	ImGui::SameLine();
	if (ImGui::Button("Edit") && App->scripting->GetFocusedScriptResource() != this)
	{
		App->scripting->SetFocusedScriptResource(this);
		App->scripting->PlaceFocusedScriptOnEditor();
	}
	ImGui::TextWrapped(buffer);
}
