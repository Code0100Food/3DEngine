#ifndef _COMPONENT_MATERIAL_H_
#define _COMPONENT_MATERIAL_H_

#include "Component.h"
#include <vector>
#include "Mesh_.h"

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

#endif
