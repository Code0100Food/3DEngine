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
	resource_script = script;
	script_object = App->scripting->CreateMonoObjectFromScriptResource(script);
	std::vector<ScriptField>* fields_vec = script->GetFields();
	uint size = fields_vec->size();
	for (uint k = 0; k < size; k++)
	{
		fields.push_back(fields_vec->at(k));
	}
	script->AddReference();
}

void ComponentScript::SetMonoObject(const MonoObject * obj)
{
	script_object = (MonoObject*)obj;
}

MonoObject * ComponentScript::GetMonoObject() const
{
	return script_object;
}

// Functionality ================================
void ComponentScript::BlitComponentInspector()
{
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Script");

	ImGui::Text("Fields:");
	uint size = fields.size();
	for (uint k = 0; k < size; k++)
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
			ImGui::InputInt(fields[k].name.c_str(),(int*)fields[k].data);
			break;

		case UINT8_FIELD:
		case UINT16_FIELD:
		case UINT32_FIELD:
		case UINT64_FIELD:
			break;

		case FLOAT_FIELD:
			ImGui::InputFloat(fields[k].name.c_str(), (float*)fields[k].data);
			break;

		case BOOL_FIELD:
			ImGui::Checkbox(fields[k].name.c_str(), (bool*)fields[k].data);
			break;

		case CLASS_FIELD:
			break;
		}
	}

	ImGui::NewLine();

	ImGui::Text("%s", resource_script->GetOwnFile());
	ImGui::SameLine();
	
	if (ImGui::Button("Edit"))
	{
		App->scripting->SetFocusedScriptResource(resource_script);
		App->scripting->PlaceFocusedScriptOnEditor();
	}
}
