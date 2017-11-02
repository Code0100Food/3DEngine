#ifndef _COMPONENT_CYLINDER_MESH_
#define _COMPONENT_CYLINDER_MESH_

#include "ComponentPrimitiveMesh.h"

class ComponentCylinderMesh : public ComponentPrimitiveMesh
{
public:

	ComponentCylinderMesh();
	ComponentCylinderMesh(const ComponentCylinderMesh& cpy);
	~ComponentCylinderMesh();

private:

	math::Cylinder	geometry;

public:

	//Set Methods -----------
	void SetGeometry(const math::Cylinder& geom);

	//Functionality ---------
	void BlitComponentInspector();

	//Save/Load Methods -----
	bool Save(Serializer& array_root)const;
	bool Load(Serializer& data, std::vector<std::pair<Component*, uint>>& links);

};
#endif // !_COMPONENT_CUBE_MESH_
