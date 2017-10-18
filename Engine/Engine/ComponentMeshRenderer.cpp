#include "ComponentMeshRenderer.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "GeometryManager.h"
#include "GameObject.h"
#include "ModuleRenderer3D.h"

// Constructors =================================
ComponentMeshRenderer::ComponentMeshRenderer():Component(COMP_MESH_RENDERER)
{

}

ComponentMeshRenderer::ComponentMeshRenderer(const ComponentMeshRenderer & cpy) : Component(cpy), render_flags(cpy.render_flags), target_mesh(cpy.target_mesh)
{

}

// Destructors ==================================
ComponentMeshRenderer::~ComponentMeshRenderer()
{
	target_mesh = nullptr;
}

// Game Loop ====================================
bool ComponentMeshRenderer::Update()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//Transform to rotate/move/scale mesh
	ComponentTransform* tmp = (ComponentTransform*)parent->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);

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
		ComponentMaterial* mat = target_mesh->GetDrawMaterial();
		if (mat!= nullptr && mat->GetActive())
		{
			for (int i = 0; i < mat->textures.size(); i++)
			{
				glBindTexture(GL_TEXTURE_2D, mat->textures[i].id);

			}
		}
	}

	//Transform the mesh before drawing
	if (tmp)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		//glLoadIdentity();
		//glLoadMatrixf(tmp->GetTransformMatrixColumns());
		glMultMatrixf(tmp->GetTransformMatrixColumns());
		
	}

	//Draw the mesh
	glLineWidth(App->geometry->mesh_lines_width);
	glColor4f(App->geometry->mesh_color[0], App->geometry->mesh_color[1], App->geometry->mesh_color[2], App->geometry->mesh_color[3]);
	glBindBuffer(GL_ARRAY_BUFFER, target_mesh->VertexBufferObject);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), NULL);
	glNormalPointer(GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normals));
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, target_mesh->ElementBufferObject);
	glDrawElements(GL_TRIANGLES, target_mesh->indices.size(), GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	glBindTexture(GL_TEXTURE_2D, 0);

	

	//Draw mesh debug information
	App->renderer3D->DisableGLRenderFlags();
	if (render_flags & REND_FACE_NORMALS)DrawFaceNormals();
	if (render_flags & REND_VERTEX_NORMALS)DrawVertexNormals();
	App->renderer3D->EnableGLRenderFlags();


	if (tmp)
		glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_ELEMENT_ARRAY_BUFFER);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	return true;
}

// Functionality ================================
void ComponentMeshRenderer::DrawVertexNormals() const
{
	if (target_mesh->vertex_normalsID == 0)return;

	//Draw vertex normals
	glBindBuffer(GL_ARRAY_BUFFER, target_mesh->vertex_normalsID);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glColor4f(App->geometry->vertex_normals_color[0], App->geometry->vertex_normals_color[1], App->geometry->vertex_normals_color[2], App->geometry->vertex_normals_color[3]);
	glLineWidth(2.f);
	glDrawArrays(GL_LINES, 0, target_mesh->vertices.size() * 2);

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ComponentMeshRenderer::DrawFaceNormals() const
{
	if (target_mesh->face_normalsID == 0)return;

	//Draw face normals
	glBindBuffer(GL_ARRAY_BUFFER, target_mesh->face_normalsID);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glColor4f(App->geometry->face_normals_color[0], App->geometry->face_normals_color[1], App->geometry->face_normals_color[2], App->geometry->face_normals_color[3]);
	glLineWidth(2.f);
	glDrawArrays(GL_LINES, 0, target_mesh->num_tris * 2);

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Set Methods ==================================
void ComponentMeshRenderer::SetTargetMesh(const ComponentMesh * target)
{
	target_mesh = (ComponentMesh*)target;
}

// Functionality ================================
void ComponentMeshRenderer::BlitComponentInspector()
{
	ImGui::Separator();

	ImGui::Checkbox("##mesh_renderer_comp", &actived);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Mesh Renderer");
	
	//Show mesh render flags
	bool rend_bool = bool(render_flags & REND_FACE_NORMALS);
	char str_buff[30];

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