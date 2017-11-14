#include "ComponentMaterial.h"
#include "Serializer.h"
#include "Application.h"
#include "FileSystem.h"
#include "ImporterManager.h"
#include "ResourcesManager.h"

// Constructors =================================
ComponentMaterial::ComponentMaterial() :Component(COMPONENT_TYPE::COMP_MATERIAL)
{

}

ComponentMaterial::ComponentMaterial(const ComponentMaterial & cpy) : Component(cpy)
{

}

// Destructors ==================================
ComponentMaterial::~ComponentMaterial()
{

}

// Set Methods ==================================
void ComponentMaterial::SetTextures(std::vector<ResourceMaterial*> textures)
{
	this->textures = textures;
}

// Functionality ================================
void ComponentMaterial::BlitComponentInspector()
{
	ImGui::Separator();

	ImGui::Checkbox("##material_comp", &actived);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Material");

	uint size = textures.size();
	for (uint k = 0; k < size; k++)
	{
		textures[k]->BlitUI();
	}
}

void ComponentMaterial::AddTexture(ResourceMaterial* tex)
{
	textures.push_back((ResourceMaterial*)tex);
	tex->AddReference();
}

bool ComponentMaterial::Save(Serializer & array_root) const
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

	//Insert all the textures data
	Serializer textures_array = comp_data.InsertArray("textures");
	uint size = textures.size();
	for (uint k = 0; k < size; k++)
	{
		//Only save the id of the resource
		textures_array.InsertArrayInt(textures[k]->GetID());
	}

	//Save the built data in the components array
	ret = array_root.InsertArrayElement(comp_data);

	return ret;
}

bool ComponentMaterial::Load(Serializer & data, std::vector<std::pair<Component*, uint>>& links)
{
	bool ret = false;

	//Get component id
	id = data.GetInt("id");
	//Get actived
	actived = data.GetBool("actived");

	//Insert all the textures data
	Serializer textures_array = data.GetArray("textures");
	uint size = textures_array.GetArraySize();
	for (uint k = 0; k < size; k++)
	{
		//Find the resources with the saved ids
		AddTexture((ResourceMaterial*)App->res_manager->Find(textures_array.GetArrayInt(k)));
	}

	return ret;
}
