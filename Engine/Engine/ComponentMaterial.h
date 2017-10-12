#ifndef _COMPONENT_MATERIAL_H_
#define _COMPONENT_MATERIAL_H_

#include "Component.h"
#include <string>
#include <vector>

struct Texture
{
	uint		id = 0;
	std::string type;
	std::string path;

	int		tex_width = 0;
	int		tex_height = 0;
};

class ComponentMaterial : public Component
{
public:

	ComponentMaterial();
	ComponentMaterial(const ComponentMaterial& cpy);
	~ComponentMaterial();

private:

	std::vector<Texture> textures;

};

#endif // !_COMPONENT_MATERIAL_H_

