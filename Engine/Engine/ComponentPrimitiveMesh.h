#ifndef _COMPONENT_PRIMITIVE_MESH_
#define _COMPONENT_PRIMITIVE_MESH_

#include "ComponentMesh.h"
#include "Primitive_.h"

class ComponentPrimitiveMesh : public ComponentMesh
{
public:

	ComponentPrimitiveMesh();
	ComponentPrimitiveMesh(const ComponentPrimitiveMesh& cpy);
	~ComponentPrimitiveMesh();

protected:

	PRIMITIVE_TYPE primitive_type = UNDEF_PRIMITIVE;

public:

	//Set Methods -----------
	void SetPrimitiveType(PRIMITIVE_TYPE type);

	//Get Methods -----------
	PRIMITIVE_TYPE GetPrimitiveType()const;

};
#endif // !_COMPONENT_PRIMITIVE_MESH_
