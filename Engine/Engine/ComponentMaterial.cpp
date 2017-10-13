#include "ComponentMaterial.h"

<<<<<<< HEAD
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
=======
ComponentMaterial::ComponentMaterial()
{
}

ComponentMaterial::ComponentMaterial(const ComponentMaterial & cpy) : textures(cpy.textures)
{
}

ComponentMaterial::~ComponentMaterial()
{
	textures.clear();
>>>>>>> origin/master
}
