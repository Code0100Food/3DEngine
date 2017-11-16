#ifndef _COMPONENT_MATERIAL_H_
#define _COMPONENT_MATERIAL_H_

#include "ComponentMesh.h"
#include <vector>

class ResourceMaterial;

class ComponentMaterial : public Component
{

	friend class ComponentMeshRenderer;

public:

	ComponentMaterial();
	ComponentMaterial(const ComponentMaterial& cpy);
	~ComponentMaterial();

private:

	std::vector<ResourceMaterial*> textures;

public:

	//Set Methods -----------
	void SetTextures(std::vector<ResourceMaterial*> textures);

	//Functionality ---------
	void BlitComponentInspector();
	void AddTexture(ResourceMaterial* tex, bool ad_ref = true);
	void RestTexture(ResourceMaterial* tex);
	void ClearTextures();

	//Save/Load Methods -----
	bool Save(Serializer& array_root)const;
	bool Load(Serializer& data, std::vector<std::pair<Component*, uint>>& links);

};
#endif // !_COMPONENT_MATERIAL_H_
