#include "ResourceScript.h"

#include "imgui/imgui.h"
#include "Application.h"
#include "ModuleScripting.h"

/// ScriptField -------------------------------------------
// Constructors =================================
ScriptField::ScriptField()
{

}

ScriptField::ScriptField(const ScriptField & cpy) :name(cpy.name), type(cpy.type)
{
	data = cpy.data; /*This can/t work*/
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

MonoAssemblyName * ResourceScript::GetAssembly() const
{
	return assembly;
}

std::vector<ScriptField>* ResourceScript::GetFields()
{
	return &fields;
}

// Functionality ================================
void ResourceScript::ClearFields()
{
	uint size = fields.size();
	for (uint k = 0; k < size; k++)
	{
		//if (fields[k].data != nullptr)RELEASE(fields[k].data);
		fields[k].data = nullptr;
		fields[k].name.clear();
	}
	fields.clear();
}

void ResourceScript::AddField(const char * name, FIELD_TYPE type, void* value)
{
	ScriptField new_field;
	
	new_field.name = name;
	new_field.type = type;
	new_field.data = value;

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
		ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.2f, 1.0f), "%s [%s] = ", fields[k].name.c_str(),App->scripting->FieldTypeToStr(fields[k].type));
		
		ImGui::SameLine();
		
		if (fields[k].data == nullptr)
		{
			ImGui::Text("null");
		}
		else
		{
			switch (fields[k].type)
			{
			case CHAR_FIELD:
				break;

			case STRING_FIELD:
				break;

			case INT8_FIELD:
			case INT16_FIELD:
			case INT32_FIELD:
			case INT64_FIELD:
				ImGui::TextColored(ImVec4(1.0f, 0.24f, 0.6f, 1.0f), "%i", *(int*)fields[k].data);
				break;

			case UINT8_FIELD:
			case UINT16_FIELD:
			case UINT32_FIELD:
			case UINT64_FIELD:
				break;

			case FLOAT_FIELD:
				ImGui::TextColored(ImVec4(1.0f, 0.24f, 0.6f, 1.0f), "%.3f", *(float*)fields[k].data);
				break;

			case BOOL_FIELD:
				if (*(bool*)fields[k].data)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.24f, 0.6f, 1.0f), "true");
				}
				else
				{
					ImGui::TextColored(ImVec4(1.0f, 0.24f, 0.6f, 1.0f), "false");
				}
				break;

			case CLASS_FIELD:
				break;
			}
		}
	}

	ImGui::NewLine();
	
	ImGui::TextWrapped(buffer);
}
