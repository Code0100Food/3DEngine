#include "ComponentPrimitiveMesh.h"

// Constructors =================================
ComponentPrimitiveMesh::ComponentPrimitiveMesh()
{
	type = COMP_PRIMITIVE_MESH;
}

ComponentPrimitiveMesh::ComponentPrimitiveMesh(const ComponentPrimitiveMesh & cpy) :ComponentMesh(cpy), divisions(cpy.divisions)
{
	type = COMP_PRIMITIVE_MESH;
}

// Destructors ==================================
ComponentPrimitiveMesh::~ComponentPrimitiveMesh()
{

}

// Set Methods ==================================
void ComponentPrimitiveMesh::SetDivisions(uint val)
{
	divisions = val;
}
