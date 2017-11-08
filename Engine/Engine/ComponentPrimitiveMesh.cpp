#include "ComponentPrimitiveMesh.h"

// Constructors =================================
ComponentPrimitiveMesh::ComponentPrimitiveMesh()
{
	type = COMP_PRIMITIVE_MESH;
}

ComponentPrimitiveMesh::ComponentPrimitiveMesh(const ComponentPrimitiveMesh & cpy) :ComponentMesh(cpy)
{
	type = COMP_PRIMITIVE_MESH;
}

// Destructors ==================================
ComponentPrimitiveMesh::~ComponentPrimitiveMesh()
{

}

void ComponentPrimitiveMesh::SetPrimitiveType(PRIMITIVE_TYPE type)
{
	primitive_type = type;
}

PRIMITIVE_TYPE ComponentPrimitiveMesh::GetPrimitiveType() const
{
	return primitive_type;
}
