#include "ComponentMaterial.h"
#include "Serializer.h"
#include "Application.h"
#include "FileSystem.h"
#include "ImporterManager.h"

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

void ComponentMaterial::AddTexture(const Texture & tex)
{
	textures.push_back(tex);
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
		Serializer texture_data = textures_array.GetArrayElement(k);
		
		char str[80];
		sprintf(str, "%s%s.dds", LIBRARY_TEXTURES_FOLDER, texture_data.GetString("path"));
		ret = App->importer->material_importer.Load(str, this);
		if (ret)
		{
			textures.back().type = texture_data.GetString("type");
			textures.back().path = texture_data.GetString("path");
		}
		if (!ret)break;
	}

	return ret;
}
