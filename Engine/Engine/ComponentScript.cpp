#include "ComponentScript.h"
#include "ModuleScene.h"
#include "ResourceScript.h"
#include "Application.h"
#include "ModuleScripting.h"
#include "Serializer.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "ResourcesManager.h"

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

bool ComponentScript::Start()
{
	SendFieldsValuesToScript();
	
	if (App->scene->GetSceneState() == SCENE_UPDATE_STATE::PLAY_SCENE_STATE)
	{
		App->scripting->SetCurrentScript(this);
		App->scripting->ExecuteMethod(script_object, "Start");
	}

	return true;
}

bool ComponentScript::Update(float dt)
{

	if (App->scene->GetSceneState() == SCENE_UPDATE_STATE::PLAY_SCENE_STATE)
	{
		App->scripting->SetCurrentScript(this);
		App->scripting->ExecuteMethod(script_object, "Update");
	}

	return true;
}

bool ComponentScript::ScriptUpdate()
{
	App->scripting->ExecuteMethod(script_object, "whatthehellman");
	return true;
}

void ComponentScript::SetResourceScript(ResourceScript * script)
{
	fields.clear();

	resource_script = script;
	script_object = App->scripting->CreateMonoObjectFromScriptResource(script);
	std::vector<ScriptField>* fields_vec = script->GetFields();
	uint size = fields_vec->size();
	for (uint k = 0; k < size; k++)
	{
		fields.push_back(fields_vec->at(k));
		fields.back().CloneData();
	}
	script->AddReference();
}

void ComponentScript::SetMonoObject(const MonoObject * obj)
{
	script_object = (MonoObject*)obj;
}

void ComponentScript::SetTargetObject(const GameObject * target)
{
	target_object = (GameObject*)target;
}

MonoObject * ComponentScript::GetMonoObject() const
{
	return script_object;
}

ResourceScript * ComponentScript::GetResourceScript() const
{
	return resource_script;
}

GameObject * ComponentScript::GetTargetObject() const
{
	return target_object;
}

// Functionality ================================
void ComponentScript::UpdateFieldsFromResource()
{
	std::vector<ScriptField>* res_fields = resource_script->GetFields();
	uint res_fields_size = res_fields->size();

	//Deleted fields check
	//Iterate all the fields of this component
	uint size = fields.size();
	for (uint k = 0; k < size; k++)
	{
		bool found = false;
		
		//Iterate all the fields of the resource and compare them whit the component fields
		for (uint j = 0; j < res_fields_size; j++)
		{
			if (fields[k].type == res_fields->at(j).type && strcmp(fields[k].name.c_str(), res_fields->at(j).name.c_str()) == 0)
			{
				found = true;
				break;
			}
		}

		//If a field exists in the component but not in the resource we delete it
		if (!found)
		{
			if (k < size - 1)fields[k] = fields[k + 1]; 
			fields.pop_back();
			size -= 1;
		}
	}

	//Added fields check
	//Iterate all the fields of the resource
	size = res_fields_size;
	uint fields_size = fields.size();
	
	if (res_fields_size == fields_size)return;

	for (uint k = 0; k < size; k++)
	{
		bool found = false;
		for (uint j = 0; j < fields_size; j++)
		{
			if (res_fields->at(k).type == fields[j].type && strcmp(res_fields->at(k).name.c_str(), fields[j].name.c_str()))
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			fields.push_back(res_fields->at(k));
			fields.back().CloneData();
			fields_size += 1;
		}
	}
}

void ComponentScript::SendFieldsValuesToScript()
{
	uint size = fields.size();
	for (uint k = 0; k < size; k++)
	{
		if (!App->scripting->SetFieldValue(script_object, fields[k].name.c_str(), fields[k].data))
		{
			App->scripting->BlitScriptingError();
		}
	}
}

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
		case INT8_FIELD:
		case INT16_FIELD:
		case INT32_FIELD:
		case INT64_FIELD:
			ImGui::InputInt(fields[k].name.c_str(), (int*)fields[k].data);
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

		case OBJECT_FIELD:
			if (fields[k].data != nullptr && fields[k].setted)
			{
				ImGui::Text(fields[k].name.c_str());
				ImGui::SameLine();
				ImGui::Text(((GameObject*)fields[k].data)->GetName());
				if (ImGui::Button("Clean Object"))
				{
					fields[k].data = nullptr;
					target_object = nullptr;
				}
			}
			else
			{
				if (ImGui::TreeNodeEx("GameObjects"))
				{
					GameObject* tar = App->scene->BlitGameObjectsAsSelectables();
					if (tar != nullptr)
					{
						fields[k].data = tar;
						fields[k].setted = true;
						target_object = (GameObject*)fields[k].data;
					}
					ImGui::TreePop();
				}

			}
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

bool ComponentScript::Save(Serializer & array_root) const
{
	bool ret = false;

	//Serializer where all the data of the component is built
	Serializer comp_data;

	//Insert Component Type
	ret = comp_data.InsertString("type", ComponentTypeToStr(type));
	//Insert component id
	ret = comp_data.InsertInt("id", id);
	//Insert actived
	ret = comp_data.InsertBool("actived", actived);

	//Insert the resource script ID
	comp_data.InsertInt("resource_script_id", resource_script->GetID());

	//Insert GameObjects array
	Serializer fields_array(comp_data.InsertArray("Fields"));

	uint size = fields.size();
	for (uint k = 0; k < size; k++)
	{
		//Current field serializer
		Serializer field_data;

		//Field name
		field_data.InsertString("name", fields[k].name.c_str());
		//Field type
		field_data.InsertString("type", App->scripting->FieldTypeToStr(fields[k].type));
		//Field size
		field_data.InsertInt("size", fields[k].data_size);
		//Field value
		switch (fields[k].type)
		{
		case INT8_FIELD:
		case INT16_FIELD:
		case INT32_FIELD:
		case INT64_FIELD:	
		case UINT8_FIELD:
		case UINT16_FIELD:
		case UINT32_FIELD:
		case UINT64_FIELD:	field_data.InsertInt("value", *(int*)fields[k].data);		break;

		case FLOAT_FIELD:	field_data.InsertFloat("value", *(float*)fields[k].data);	break;
		case BOOL_FIELD:	field_data.InsertBool("value", *(bool*)fields[k].data);		break;

		case OBJECT_FIELD:	field_data.InsertInt("value", ((GameObject*)fields[k].data)->GetID());	break;
		}
		
		//Insert the generated field serialization
		fields_array.InsertArrayElement(field_data);
	}

	//Save the built data in the components array
	ret = array_root.InsertArrayElement(comp_data);

	return ret;
}

bool ComponentScript::Load(Serializer & data, std::vector<std::pair<Component*, uint>>& links)
{
	bool ret = true;

	//Get component id
	id = data.GetInt("id");
	//Get actived
	actived = data.GetBool("actived");

	//Get mesh file path
	ResourceScript* script_res = (ResourceScript*)App->res_manager->Find(data.GetInt("resource_script_id"));
	if (script_res == nullptr)return false;	
	SetResourceScript(script_res);

	//Get the fields array
	Serializer fields_array = data.GetArray("Fields");
	uint size = fields_array.GetArraySize();

	for(uint k = 0; k < size; k++)
	{
		Serializer field_data = fields_array.GetArrayElement(k);
		
		if (strcmp(fields[k].name.c_str(), field_data.GetString("name")) == 0 && fields[k].type == App->scripting->StrToFieldType(field_data.GetString("type")))
		{
			switch (fields[k].type)
			{
			case INT8_FIELD:
			case INT16_FIELD:
			case INT32_FIELD:
			case INT64_FIELD:
			case UINT8_FIELD:
			case UINT16_FIELD:
			case UINT32_FIELD:
			case UINT64_FIELD:	*(int*)fields[k].data = field_data.GetInt("value");		break;

			case FLOAT_FIELD:	*(float*)fields[k].data = field_data.GetFloat("value");	break;
			case BOOL_FIELD:	*(bool*)fields[k].data = field_data.GetBool("value");	break;

			case OBJECT_FIELD:	fields[k].data = App->scene->FindGameObject(field_data.GetInt("value")); fields[k].setted = true; target_object = (GameObject*)fields[k].data;	break;
			} 
		}
	}
	
	return ret;
}

//MONO METHODS

//Transform
MonoObject* ComponentScript::GetLocalPosition()
{
	//Returned value to the c# script
	MonoObject* ret = App->scripting->CreateMonoObject(App->scripting->GetEnviromentAssembly(), "FiestaVector3", "FiestaEngine");

	//Get the fields of this MonoObject Type
	MonoClassField* x_value = App->scripting->GetFieldFromImage(App->scripting->GetEnviromentImage(), "FiestaEngine", "FiestaVector3", "x");
	MonoClassField* y_value = App->scripting->GetFieldFromImage(App->scripting->GetEnviromentImage(), "FiestaEngine", "FiestaVector3", "y");
	MonoClassField* z_value = App->scripting->GetFieldFromImage(App->scripting->GetEnviromentImage(), "FiestaEngine", "FiestaVector3", "z");

	if (ret)
	{
		//Get the transform and set the values
		ComponentTransform* trans = (ComponentTransform*)parent->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
		math::float3 position = trans->GetInheritedTransform().Transposed().TranslatePart();

		App->scripting->SetFieldValue(ret, x_value, &position.x);
		App->scripting->SetFieldValue(ret, y_value, &position.y);
		App->scripting->SetFieldValue(ret, z_value, &position.z);

		return ret;
	}
	
	return nullptr;
}

void ComponentScript::SetPosition(MonoObject* vector)
{

	void* x_value = nullptr;
	App->scripting->GetFieldValue(vector, "x", &x_value);

	void* y_value = nullptr;
	App->scripting->GetFieldValue(vector, "y", &y_value);

	void* z_value = nullptr;
	App->scripting->GetFieldValue(vector, "z", &z_value);

	float x = *(float*)x_value;
	float y = *(float*)y_value;
	float z = *(float*)z_value;

	ComponentTransform* trans = (ComponentTransform*)parent->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
	trans->SetPosition(x, y, z);

}

void ComponentScript::RotateFromEulerAngles(MonoObject* e_angles)
{
	void* x_value = nullptr;
	App->scripting->GetFieldValue(e_angles, "x", &x_value);

	void* y_value = nullptr;
	App->scripting->GetFieldValue(e_angles, "y", &y_value);

	void* z_value = nullptr;
	App->scripting->GetFieldValue(e_angles, "z", &z_value);

	float x = *(float*)x_value;
	float y = *(float*)y_value;
	float z = *(float*)z_value;

	ComponentTransform* trans = (ComponentTransform*)parent->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
	trans->RotateFromEulerAngles(x, y, z);
}

MonoObject * ComponentScript::GetForwardDirection()
{
	//Returned value to the c# script
	MonoObject* ret = App->scripting->CreateMonoObject(App->scripting->GetEnviromentAssembly(), "FiestaVector3", "FiestaEngine");

	//Get the fields of this MonoObject Type
	MonoClassField* x_value = App->scripting->GetFieldFromImage(App->scripting->GetEnviromentImage(), "FiestaEngine", "FiestaVector3", "x");
	MonoClassField* y_value = App->scripting->GetFieldFromImage(App->scripting->GetEnviromentImage(), "FiestaEngine", "FiestaVector3", "y");
	MonoClassField* z_value = App->scripting->GetFieldFromImage(App->scripting->GetEnviromentImage(), "FiestaEngine", "FiestaVector3", "z");

	if (ret)
	{
		//Get the transform and set the values
		ComponentTransform* trans = (ComponentTransform*)parent->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
		math::float3 position = trans->GetInheritedTransform().Transposed().Col3(2);

		App->scripting->SetFieldValue(ret, x_value, &position.x);
		App->scripting->SetFieldValue(ret, y_value, &position.y);
		App->scripting->SetFieldValue(ret, z_value, &position.z);

		return ret;
	}

	return nullptr;;
}

MonoObject * ComponentScript::GetRightDirection()
{
	//Returned value to the c# script
	MonoObject* ret = App->scripting->CreateMonoObject(App->scripting->GetEnviromentAssembly(), "FiestaVector3", "FiestaEngine");

	//Get the fields of this MonoObject Type
	MonoClassField* x_value = App->scripting->GetFieldFromImage(App->scripting->GetEnviromentImage(), "FiestaEngine", "FiestaVector3", "x");
	MonoClassField* y_value = App->scripting->GetFieldFromImage(App->scripting->GetEnviromentImage(), "FiestaEngine", "FiestaVector3", "y");
	MonoClassField* z_value = App->scripting->GetFieldFromImage(App->scripting->GetEnviromentImage(), "FiestaEngine", "FiestaVector3", "z");

	if (ret)
	{
		//Get the transform and set the values
		ComponentTransform* trans = (ComponentTransform*)parent->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
		math::float3 position = trans->GetInheritedTransform().Transposed().Col3(0);

		App->scripting->SetFieldValue(ret, x_value, &position.x);
		App->scripting->SetFieldValue(ret, y_value, &position.y);
		App->scripting->SetFieldValue(ret, z_value, &position.z);

		return ret;
	}

	return nullptr;
}
