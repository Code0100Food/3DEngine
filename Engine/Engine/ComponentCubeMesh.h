#ifndef _COMPONENT_CUBE_MESH_
#define _COMPONENT_CUBE_MESH_

#include "ComponentPrimitiveMesh.h"

class ComponentCubeMesh : public ComponentPrimitiveMesh
{
public:

	ComponentCubeMesh();
	ComponentCubeMesh(const ComponentCubeMesh& cpy);
	~ComponentCubeMesh();

private:

	math::AABB	geometry;
	
public:

	//Set Methods -----------
	void SetGeometry(const math::AABB& geom);

	//Functionality ---------
	void BlitComponentInspector();

	//Save/Load Methods -----
	bool Save(Serializer& array_root)const;
	bool Load(Serializer& data, std::vector<std::pair<Component*, uint>>& links);

};
#endif // !_COMPONENT_CUBE_MESH_
