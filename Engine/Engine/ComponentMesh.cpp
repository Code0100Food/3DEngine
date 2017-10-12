#include "ComponentMesh.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

ComponentMesh::ComponentMesh()
{
}

ComponentMesh::ComponentMesh(const ComponentMesh & cpy) : vertices(cpy.vertices), indices(cpy.indices), bounding_box(cpy.bounding_box)
{	
}

ComponentMesh::ComponentMesh(std::vector<Vertex> vertices, std::vector<uint> indices, ComponentMaterial * material) : vertices(vertices), indices(indices)
{
	if (indices.size() % 3 == 0)num_tris = indices.size() / 3;
}

ComponentMesh::~ComponentMesh()
{
}

void ComponentMesh::SetMesh()
{
	glGenBuffers(1, &VertexBufferObject);
	glGenBuffers(1, &ElementBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	num_vertex = vertices.size();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);

	//Build mesh vertex normals
	std::vector<math::float3> vertex_normals;
	for (uint k = 0; k < num_vertex; k++)
	{
		vertex_normals.push_back(vertices.data()[k].position);
		vertex_normals.push_back((vertices.data()[k].position + vertices.data()[k].normals));
	}

	glGenBuffers(1, &vertex_normalsID);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_normalsID);
	glBufferData(GL_ARRAY_BUFFER, vertex_normals.size() * sizeof(math::float3), &vertex_normals.data()[0], GL_STATIC_DRAW);

	//Build mesh face normals
	if (num_tris != 0)
	{
		std::vector<math::float3> face_normals;
		uint size = indices.size();
		for (uint k = 0; k < size - 2; k += 3)
		{
			math::float3 V(vertices[indices[k + 1]].position.x - vertices[indices[k]].position.x, vertices[indices[k + 1]].position.y - vertices[indices[k]].position.y, vertices[indices[k + 1]].position.z - vertices[indices[k]].position.z);
			math::float3 W(vertices[indices[k + 2]].position.x - vertices[indices[k]].position.x, vertices[indices[k + 2]].position.y - vertices[indices[k]].position.y, vertices[indices[k + 2]].position.z - vertices[indices[k]].position.z);
			math::float3 normal(V.Cross(W));
			normal.Normalize();
			math::float3 center_point((vertices[indices[k]].position.x + vertices[indices[k + 1]].position.x + vertices[indices[k + 2]].position.x) / 3, (vertices[indices[k]].position.y + vertices[indices[k + 1]].position.y + vertices[indices[k + 2]].position.y) / 3, (vertices[indices[k]].position.z + vertices[indices[k + 1]].position.z + vertices[indices[k + 2]].position.z) / 3);
			face_normals.push_back(center_point);
			face_normals.push_back(center_point + normal);
		}

		glGenBuffers(1, &face_normalsID);
		glBindBuffer(GL_ARRAY_BUFFER, face_normalsID);
		glBufferData(GL_ARRAY_BUFFER, face_normals.size() * sizeof(math::float3), &face_normals.data()[0], GL_STATIC_DRAW);
	}

}

void ComponentMesh::GenerateBoundingBox()
{
	math::float3 min_pos(0, 0, 0);
	math::float3 max_pos(0, 0, 0);

	if (vertices.size() > 0) {
		min_pos = vertices.data()[0].position;
		max_pos = vertices.data()[0].position;
	}

	for (uint k = 0; k < vertices.size(); k++)
	{
		min_pos.x = MIN(min_pos.x, vertices.data()[k].position.x);
		min_pos.y = MIN(min_pos.y, vertices.data()[k].position.y);
		min_pos.z = MIN(min_pos.z, vertices.data()[k].position.z);
		max_pos.x = MAX(max_pos.x, vertices.data()[k].position.x);
		max_pos.y = MAX(max_pos.y, vertices.data()[k].position.y);
		max_pos.z = MAX(max_pos.z, vertices.data()[k].position.z);
	}

	//Save the vertex of the built bounding box
	math::AABB b_b;
	b_b.minPoint = min_pos;
	b_b.maxPoint = max_pos;
	std::vector<math::float3> temp_vec;
	temp_vec.reserve(8);
	b_b.GetCornerPoints(temp_vec.data());
	for (uint k = 0; k < 8; k++)bounding_box.push_back(temp_vec.data()[k]);
	temp_vec.clear();
}

void ComponentMesh::DeleteBuffers()
{
	if (VertexArrayObject != NULL)	glDeleteBuffers(1, &VertexArrayObject);
	if (VertexBufferObject != NULL)	glDeleteBuffers(1, &VertexBufferObject);
	if (ElementBufferObject != NULL)glDeleteBuffers(1, &ElementBufferObject);
	if (face_normalsID != NULL)		glDeleteBuffers(1, &face_normalsID);
	if (vertex_normalsID != NULL)	glDeleteBuffers(1, &vertex_normalsID);
	if (text_coordsID != NULL)		glDeleteBuffers(1, &text_coordsID);
}
