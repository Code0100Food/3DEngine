#include "ComponentMaterial.h"

ComponentMaterial::ComponentMaterial()
{
}

ComponentMaterial::ComponentMaterial(const ComponentMaterial & cpy) : textures(cpy.textures)
{
}

ComponentMaterial::~ComponentMaterial()
{
	textures.clear();
}
