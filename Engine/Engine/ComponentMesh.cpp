#include "ComponentMesh.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include "ComponentMaterial.h"
#include "Serializer.h"
#include "Application.h"
#include "ImporterManager.h"

// Vertex ---------------------------------------
Vertex::Vertex()
{

}

Vertex::Vertex(math::float3 position, math::float3 normals, math::float2 tex_coords): position(position), normals(normals), tex_coords(tex_coords)
{

}
// ----------------------------------------------

// Texture --------------------------------------
void Texture::BlitUI()const
{
	ImGui::Text("%s", type.c_str());
	ImGui::Image((void*)id, ImVec2(100, 100));
	ImGui::Text("Size: %ix%i", tex_width, tex_height);
}
// ----------------------------------------------

// Constructors =================================
ComponentMesh::ComponentMesh() :Component(COMP_MESH)
{

}

ComponentMesh::ComponentMesh(const ComponentMesh & cpy) : Component(cpy), vertices(cpy.vertices), indices(cpy.indices)
{	

}

// Destructors ==================================
ComponentMesh::~ComponentMesh()
{
	vertices.clear();
	indices.clear();
	draw_material = nullptr;

	this->DeleteBuffers();
}

// Get Methods ==================================
ComponentMaterial * ComponentMesh::GetDrawMaterial() const
{
	return draw_material;
}

std::vector<math::float3> ComponentMesh::GetVertexPositions() const
{
	std::vector<math::float3> vec_pos;
	uint size = vertices.size();
	for (uint k = 0; k < size; k++)vec_pos.push_back(vertices[k].position);
	return vec_pos;
}

uint ComponentMesh::GetIndexSize() const
{
	return indices.size();
}

int ComponentMesh::GetIndexAt(int position) const
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

math::float3 ComponentMesh::GetVertexPosAt(int position) const
{
	return vertices[position].position;
}

// Set Methods ==================================
void ComponentMesh::SetVertices(std::vector<Vertex> v)
{
	vertices = v;
	num_vertex = vertices.size();
}

void ComponentMesh::SetIndices(std::vector<uint> i)
{
	indices = i;
	if (indices.size() % 3 == 0)num_tris = indices.size() / 3;
}

void ComponentMesh::SetPath(const char * str)
{
	path = str;
}

void ComponentMesh::SetDrawMaterial(ComponentMaterial * mat)
{
	draw_material = mat;
}

// Functionality ================================
void ComponentMesh::SetupMesh()
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

void ComponentMesh::DeleteBuffers()
{
	if (VertexArrayObject != NULL)	glDeleteBuffers(1, &VertexArrayObject);
	if (VertexBufferObject != NULL)	glDeleteBuffers(1, &VertexBufferObject);
	if (ElementBufferObject != NULL)glDeleteBuffers(1, &ElementBufferObject);
	if (face_normalsID != NULL)		glDeleteBuffers(1, &face_normalsID);
	if (vertex_normalsID != NULL)	glDeleteBuffers(1, &vertex_normalsID);
	if (text_coordsID != NULL)		glDeleteBuffers(1, &text_coordsID);
}

void ComponentMesh::BlitComponentInspector()
{
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Mesh");

	//Show mesh Tris & Vertex
	ImGui::Text("Tris: %i", num_tris);
	ImGui::SameLine();
	ImGui::Text("Vertex: %i", num_vertex);
}

bool ComponentMesh::Save(Serializer & array_root) const
{
	bool ret = false;

	//Serializer where all the data of the component is built
	Serializer comp_data;

	//Insert Component Type
	ret = comp_data.InsertString("type", ComponentTypeToStr(type));
	//Insert component id
	ret = comp_data.InsertInt("id", id);
	//Insert actived
	ret = comp_data.InsertBool("actived", actived);
	
	//Insert mesh file path
	comp_data.InsertString("path", path.c_str());

	//Insert draw material id
	if (draw_material != nullptr)ret = comp_data.InsertInt("draw_material_id", draw_material->GetID());

	//Save the built data in the components array
	ret = array_root.InsertArrayElement(comp_data);

	return ret;
}

bool ComponentMesh::Load(Serializer & data, std::vector<std::pair<Component*, uint>>& links)
{
	bool ret = true;

	//Get component id
	id = data.GetInt("id");
	//Get actived
	actived = data.GetBool("actived");

	//Get mesh file path
	char str[80];
	sprintf(str, "%s%s.fiesta",LIBRARY_MESH_FOLDER, data.GetString("path"));
	App->importer->mesh_importer.Load(str, this);
		
	//Get draw material id
	uint material_id = data.GetInt("draw_material_id");
	if(material_id != 0)links.push_back(std::pair<Component*, uint>(this, material_id));

	return ret;
}

void ComponentMesh::LinkComponent(const Component * target)
{
	draw_material = (ComponentMaterial*)target;
}
