#include "ComponentMaterial.h"
#include "Serializer.h"
#include "Application.h"
#include "FileSystem.h"

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
void ComponentMaterial::SetTextures(std::vector<Texture> textures)
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
		textures[k].BlitUI();
	}
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
		Serializer texture_data;
		
		//Texture path
		std::string file_name;
		App->fs->GetFileNameFromPath(textures[k].path.c_str(), &file_name);
		file_name = file_name.substr(0, file_name.length() - (file_name.length() - file_name.find_last_of(".")));

		texture_data.InsertString("path", file_name.c_str());
		texture_data.InsertString("type", textures[k].type.c_str());

		textures_array.InsertArrayElement(texture_data);
	}

	//Save the built data in the components array
	ret = array_root.InsertArrayElement(comp_data);

	return ret;
}
