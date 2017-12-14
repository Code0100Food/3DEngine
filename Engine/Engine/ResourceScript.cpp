#include "ResourceScript.h"

#include "imgui/imgui.h"
#include "Application.h"
#include "ModuleScripting.h"

/// ScriptField -------------------------------------------
// Constructors =================================
ScriptField::ScriptField()
{

}

// Destructors ==================================
ScriptField::~ScriptField()
{

}
/// -------------------------------------------------------

/// ResourceScript ----------------------------------------
// Constructors =================================
ResourceScript::ResourceScript():Resource(SCRIPT_RESOURCE)
{

}

// Destructors ==================================
ResourceScript::~ResourceScript()
{

}
// Set Methods ==================================
void ResourceScript::SetBuffer(const char * bf)
{
	if (buffer != nullptr)RELEASE_ARRAY(buffer);
	buffer = (char*)bf;
}

void ResourceScript::SetAssembly(const MonoAssemblyName * asm_)
{
	assembly = (MonoAssemblyName*)asm_;
}

// Get Methods ==================================
char * ResourceScript::GetBuffer() const
{
	return buffer;
}

std::vector<ScriptField>* ResourceScript::GetFields()
{
	return &fields;
}

// Functionality ================================
void ResourceScript::ClearFields()
{

}

void ResourceScript::AddField(const char * name, FIELD_TYPE type)
{
	ScriptField new_field;
	
	new_field.name = name;
	new_field.type = type;

	fields.push_back(new_field);
}

void ResourceScript::BlitUI() const
{

}

void ResourceScript::BlitComplexUI()
{
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%s", ResourceTypeToStr(type));
	ImGui::SameLine();
	if (ImGui::Button("Edit"))
	{
		App->scripting->SetFocusedScriptResource(this);
		App->scripting->PlaceFocusedScriptOnEditor();
	}

	ImGui::Text("Original File:");
	ImGui::Text("%s", original_file.c_str());
	ImGui::Text("Own File:");
	ImGui::Text("%s", own_file.c_str());
	ImGui::Text("References: %i", references);
	ImGui::Text("Last edition time: %i", last_edition_time);
	
	ImGui::NewLine();

	ImGui::Text("Fields:");
	uint size = fields.size();
	for (uint k = 0; k < size; k++)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.2f, 1.0f), "%s [%s]", fields[k].name.c_str(),App->scripting->FieldTypeToStr(fields[k].type));
	}

	ImGui::NewLine();
	
	ImGui::TextWrapped(buffer);
}
