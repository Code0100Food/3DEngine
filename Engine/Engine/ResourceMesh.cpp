#include "ResourceMesh.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

#include "Application.h"
#include "Serializer.h"

// Constructors =================================
ResourceMesh::ResourceMesh() :Resource(MESH_RESOURCE)
{
	//Generate a random id for the resource
	id = App->randomizer->Int();
}

// Destructors ==================================
ResourceMesh::~ResourceMesh()
{
	vertices.clear();
	indices.clear();
	
	DeleteBuffers();
}

// Get Methods ==================================
std::vector<math::float3> ResourceMesh::GetVertexPositions() const
{
	std::vector<math::float3> vec_pos;
	uint size = vertices.size();
	for (uint k = 0; k < size; k++)vec_pos.push_back(vertices[k].position);
	return vec_pos;
}

uint ResourceMesh::GetIndexSize() const
{
	return indices.size();
}

int ResourceMesh::GetIndexAt(int position) const
{
	if (position < indices.size())
	{
		return indices[position];
	}
	else
	{
		return -1;
	}
}

math::float3 ResourceMesh::GetVertexPosAt(int position) const
{
	return vertices[position].position;
}

uint ResourceMesh::GetNumTris() const
{
	return num_tris;
}

uint ResourceMesh::GetNumVertex() const
{
	return num_vertex;
}

uint ResourceMesh::GetVertexArrayObject() const
{
	return VertexArrayObject;
}

uint ResourceMesh::GetVertexBufferObject() const
{
	return VertexBufferObject;
}

uint ResourceMesh::GetElementBufferObject() const
{
	return ElementBufferObject;
}

uint ResourceMesh::GetFaceNormalsID() const
{
	return face_normalsID;
}

uint ResourceMesh::GetVertexNormalsID() const
{
	return vertex_normalsID;
}

uint ResourceMesh::GetTextureCoordsID() const
{
	return text_coordsID;
}

// Set Methods ==================================
void ResourceMesh::SetVertices(const std::vector<Vertex>& v)
{
	vertices = v;
	num_vertex = vertices.size();
}

void ResourceMesh::SetIndices(const std::vector<uint>& i)
{
	indices = i;
	if (indices.size() % 3 == 0)num_tris = indices.size() / 3;
}

// Functionality ================================
void ResourceMesh::SetupMesh()
{
	glGenBuffers(1, &VertexBufferObject);
	glGenBuffers(1, &ElementBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	num_vertex = vertices.size();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);

	//Build mesh vertex normals
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

void ResourceMesh::DeleteBuffers()
{
	if (VertexArrayObject != NULL)	glDeleteBuffers(1, &VertexArrayObject);
	if (VertexBufferObject != NULL)	glDeleteBuffers(1, &VertexBufferObject);
	if (ElementBufferObject != NULL)glDeleteBuffers(1, &ElementBufferObject);
	if (face_normalsID != NULL)		glDeleteBuffers(1, &face_normalsID);
	if (vertex_normalsID != NULL)	glDeleteBuffers(1, &vertex_normalsID);
	if (text_coordsID != NULL)		glDeleteBuffers(1, &text_coordsID);
}

bool ResourceMesh::Save(Serializer & file_root) const
{
	//Save all the standard resource data
	file_root.InsertInt("id", id);
	file_root.InsertString("res_type", ResourceTypeToStr(type));
	file_root.InsertString("original_file", original_file.c_str());
	file_root.InsertString("own_file", own_file.c_str());

	return true;
}

bool ResourceMesh::Load(Serializer & data)
{
	return true;
}

void ResourceMesh::LoadInMemory()
{
	SetupMesh();
}

void ResourceMesh::UnloadInMemory()
{
	vertex_normals.clear();
	face_normals.clear();

	DeleteBuffers();
}
