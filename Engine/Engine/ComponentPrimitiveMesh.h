#ifndef _COMPONENT_PRIMITIVE_MESH_
#define _COMPONENT_PRIMITIVE_MESH_

#include "ComponentMesh.h"

class ComponentPrimitiveMesh : public ComponentMesh
{
public:

	ComponentPrimitiveMesh();
	ComponentPrimitiveMesh(const ComponentPrimitiveMesh& cpy);
	~ComponentPrimitiveMesh();

protected:

	uint divisions = 0;

public:

	//Set Methods -----------
	void SetDivisions(uint val);

};
#endif // !_COMPONENT_PRIMITIVE_MESH_
