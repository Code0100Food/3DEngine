#ifndef _COMPONENT_SPHERE_MESH_
#define _COMPONENT_SPHERE_MESH_

#include "ComponentPrimitiveMesh.h"

class ComponentSphereMesh : public ComponentPrimitiveMesh
{
public:

	ComponentSphereMesh();
	ComponentSphereMesh(const ComponentSphereMesh& cpy);
	~ComponentSphereMesh();

private:

	math::Sphere	geometry;

public:

	//Set Methods -----------
	void SetGeometry(const math::Sphere& geom);

	//Functionality ---------
	void BlitComponentInspector();

	//Save/Load Methods -----
	bool Save(Serializer& array_root)const;

};
#endif // !_COMPONENT_SPHERE_MESH_
