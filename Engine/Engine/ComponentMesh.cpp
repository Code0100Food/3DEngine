#include "ComponentMesh.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include "ComponentMaterial.h"
#include "Application.h"
#include "GeometryManager.h"
#include "ModuleRenderer3D.h"
#include "ModuleTextures.h"

// Constructors =================================
ComponentMesh::ComponentMesh() :Component(COMP_MESH)
{

}

ComponentMesh::ComponentMesh(const ComponentMesh & cpy) : Component(cpy), vertices(cpy.vertices), indices(cpy.indices), bounding_box(cpy.bounding_box)
{	

}

ComponentMesh::ComponentMesh(std::vector<Vertex> vertices, std::vector<uint> indices, ComponentMaterial * material) : Component(COMP_MESH), vertices(vertices), indices(indices)
{
	if (indices.size() % 3 == 0)num_tris = indices.size() / 3;
}

// Destructors ==================================
ComponentMesh::~ComponentMesh()
{
	this->DeleteBuffers();
}

// Game Loop ====================================
bool ComponentMesh::Update()
{

	glBindTexture(GL_TEXTURE_2D, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//Select between the textures
	if (App->textures->GetCheckMode())
	{
		glBindTexture(GL_TEXTURE_2D, App->textures->check_image);
	}

	else if (App->textures->GetCustomMode())
	{
		glBindTexture(GL_TEXTURE_2D, App->textures->custom_check_image);
	}

	else if (App->textures->GetMeshMode())
	{
		if (draw_material->GetActive())
		{
			for (int i = 0; i < draw_material->textures.size(); i++)
			{
				glBindTexture(GL_TEXTURE_2D, draw_material->textures[i].id);

			}
		}
	}

	//Draw the mesh
	glLineWidth(App->geometry->mesh_lines_width);
	glColor4f(App->geometry->mesh_color[0], App->geometry->mesh_color[1], App->geometry->mesh_color[2], App->geometry->mesh_color[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), NULL);
	glNormalPointer(GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normals));
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	//Draw mesh debug information
	App->renderer3D->DisableGLRenderFlags();
	if (render_flags & REND_FACE_NORMALS)DrawFaceNormals();
	if (render_flags & REND_VERTEX_NORMALS)DrawVertexNormals();
	if (render_flags & REND_BOUND_BOX)DrawBoundingBox();
	App->renderer3D->EnableGLRenderFlags();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_ELEMENT_ARRAY_BUFFER);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	return true;
}

void ComponentMesh::DrawVertexNormals() const
{
	if (vertex_normalsID == 0)return;

	//Draw vertex normals
	glBindBuffer(GL_ARRAY_BUFFER, vertex_normalsID);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glColor4f(App->geometry->vertex_normals_color[0], App->geometry->vertex_normals_color[1], App->geometry->vertex_normals_color[2], App->geometry->vertex_normals_color[3]);
	glLineWidth(2.f);
	glDrawArrays(GL_LINES, 0, vertices.size() * 2);

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ComponentMesh::DrawFaceNormals() const
{
	if (face_normalsID == 0)return;

	//Draw face normals
	glBindBuffer(GL_ARRAY_BUFFER, face_normalsID);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glColor4f(App->geometry->face_normals_color[0], App->geometry->face_normals_color[1], App->geometry->face_normals_color[2], App->geometry->face_normals_color[3]);
	glLineWidth(2.f);
	glDrawArrays(GL_LINES, 0, num_tris * 2);

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ComponentMesh::DrawBoundingBox() const
{
	if (bounding_box.size() < 8)return;

	//Draw bounding box
	glBegin(GL_LINES);

	glColor4f(App->geometry->bounding_box_color[0], App->geometry->bounding_box_color[1], App->geometry->bounding_box_color[2], App->geometry->bounding_box_color[3]);
	glLineWidth(2.f);

	for (uint k = 0; k < 4; k++)
	{
		glVertex3f(bounding_box.data()[k + 4].x, bounding_box.data()[k + 4].y, bounding_box.data()[k + 4].z);
		glVertex3f(bounding_box.data()[k].x, bounding_box.data()[k].y, bounding_box.data()[k].z);
	}

	for (uint k = 0; k <= 4; k += 4)
	{
		glVertex3f(bounding_box.data()[k].x, bounding_box.data()[k].y, bounding_box.data()[k].z);
		glVertex3f(bounding_box.data()[k + 1].x, bounding_box.data()[k + 1].y, bounding_box.data()[k + 1].z);

		glVertex3f(bounding_box.data()[k + 2].x, bounding_box.data()[k + 2].y, bounding_box.data()[k + 2].z);
		glVertex3f(bounding_box.data()[k + 3].x, bounding_box.data()[k + 3].y, bounding_box.data()[k + 3].z);

		glVertex3f(bounding_box.data()[k].x, bounding_box.data()[k].y, bounding_box.data()[k].z);
		glVertex3f(bounding_box.data()[k + 2].x, bounding_box.data()[k + 2].y, bounding_box.data()[k + 2].z);

		glVertex3f(bounding_box.data()[k + 1].x, bounding_box.data()[k + 1].y, bounding_box.data()[k + 1].z);
		glVertex3f(bounding_box.data()[k + 3].x, bounding_box.data()[k + 3].y, bounding_box.data()[k + 3].z);
	}
	glEnd();
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

	//Build bounding box
	GenerateBoundingBox();
}

void ComponentMesh::GenerateBoundingBox()
{
	math::float3 min_pos(0, 0, 0);
	math::float3 max_pos(0, 0, 0);

	if (vertices.size() > 0)
	{
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

void ComponentMesh::BlitComponentInspector()
{
	ImGui::Separator();

	ImGui::Checkbox("##mesh_comp", &actived);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Mesh");

	//Show mesh Tris & Vertex
	ImGui::Text("Tris: %i", num_tris);
	ImGui::SameLine();
	ImGui::Text("Vertex: %i", num_vertex);

	ImGui::NewLine();

	//Show mesh render flags
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Render Flags");
	bool rend_bool = bool(render_flags & REND_BOUND_BOX);
	char str_buff[30];
	sprintf_s(str_buff, "Bounding Box");
	if (ImGui::Checkbox(str_buff, &rend_bool))
	{
		if (rend_bool)
		{
			render_flags |= REND_BOUND_BOX;
		}
		else
		{
			render_flags &= ~REND_BOUND_BOX;
		}

	}

	rend_bool = bool(render_flags & REND_FACE_NORMALS);
	sprintf_s(str_buff, "Face Normals");
	if (ImGui::Checkbox(str_buff, &rend_bool))
	{
		if (rend_bool)
		{
			render_flags |= REND_FACE_NORMALS;
		}
		else
		{
			render_flags &= ~REND_FACE_NORMALS;
		}

	}

	rend_bool = bool(render_flags & REND_VERTEX_NORMALS);
	sprintf_s(str_buff, "Vertex Normals");
	if (ImGui::Checkbox(str_buff, &rend_bool))
	{
		if (rend_bool)
		{
			render_flags |= REND_VERTEX_NORMALS;
		}
		else
		{
			render_flags &= ~REND_VERTEX_NORMALS;
		}

	}
}
