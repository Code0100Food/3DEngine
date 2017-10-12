#ifndef _COMPONENT_MESH_H_
#define _COMPONENT_MESH_H_

#include "Component.h"

#include "MathGeoLib/MathGeoLib.h"

struct Vertex
{
	math::float3 position = { 0,0,0 };
	math::float3 normals = { 0,0,0 };
	math::float2 tex_coords = { 0,0 };
};

class ComponentMaterial;

class ComponentMesh : public Component
{
public:

	ComponentMesh();
	ComponentMesh(const ComponentMesh& cpy);
	ComponentMesh(std::vector<Vertex> vertices, std::vector<uint> indices, ComponentMaterial* material);
	~ComponentMesh();

private:

	std::vector<Vertex>			vertices;
	std::vector<uint>			indices;
	std::vector<math::float3>	bounding_box;

	uint					num_tris = 0;
	uint					num_vertex = 0;

	ComponentMaterial* draw_material = nullptr;

private:

	uint VertexArrayObject = 0, VertexBufferObject = 0, ElementBufferObject = 0;
	uint face_normalsID = 0;
	uint vertex_normalsID = 0;
	uint text_coordsID = 0;

private:

	//Functionality-----
	void	SetMesh();
	void	GenerateBoundingBox();
	void	DeleteBuffers();


};


#endif // !_COMPONENT_MESH_H_
