#ifndef _COMPONENT_MATERIAL_H_
#define _COMPONENT_MATERIAL_H_

#include "ComponentMesh.h"
#include <vector>

class ComponentMaterial : public Component
{

	friend class ComponentMesh;

public:

	ComponentMaterial();
	ComponentMaterial(const ComponentMaterial& cpy);
	~ComponentMaterial();

private:

	std::vector<Texture> textures;

public:

	//Set Methods -----------
	void SetTextures(std::vector<Texture> textures);

	//Functionality ---------
	void BlitComponentInspector();

};
#endif // !_COMPONENT_MATERIAL_H_
