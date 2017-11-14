#include "ComponentMesh.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include "ComponentMaterial.h"
#include "Serializer.h"
#include "Application.h"
#include "ImporterManager.h"
#include "ResourceMesh.h"
#include "GameObject.h"
#include "ResourcesManager.h"

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

ComponentMesh::ComponentMesh(const ComponentMesh & cpy) : Component(cpy), resource_mesh(cpy.resource_mesh)
{	

}

// Destructors ==================================
ComponentMesh::~ComponentMesh()
{
	draw_material = nullptr;
	if (resource_mesh != nullptr)resource_mesh->RestReference();
	resource_mesh = nullptr;
}

// Get Methods ==================================
ComponentMaterial * ComponentMesh::GetDrawMaterial() const
{
	return draw_material;
}

std::vector<math::float3> ComponentMesh::GetVertexPositions() const
{
	return resource_mesh->GetVertexPositions();
}

uint ComponentMesh::GetIndexSize() const
{
	return resource_mesh->GetIndexSize();
}

int ComponentMesh::GetIndexAt(int position) const
{
	return resource_mesh->GetIndexAt(position);
}

math::float3 ComponentMesh::GetVertexPosAt(int position) const
{
	return resource_mesh->GetVertexPosAt(position);
}

uint ComponentMesh::GetNumTris() const
{
	return resource_mesh->GetNumTris();
}

uint ComponentMesh::GetNumVertex() const
{
	return resource_mesh->GetNumVertex();
}

uint ComponentMesh::GetVertexArrayObject() const
{
	return resource_mesh->GetVertexArrayObject();
}

uint ComponentMesh::GetVertexBufferObject() const
{
	return resource_mesh->GetVertexBufferObject();
}

uint ComponentMesh::GetElementBufferObject() const
{
	return resource_mesh->GetElementBufferObject();
}

uint ComponentMesh::GetFaceNormalsID() const
{
	return resource_mesh->GetFaceNormalsID();
}

uint ComponentMesh::GetVertexNormalsID() const
{
	return resource_mesh->GetVertexNormalsID();
}

uint ComponentMesh::GetTextureCoordsID() const
{
	return resource_mesh->GetTextureCoordsID();
}

// Set Methods ==================================
void ComponentMesh::SetVertices(const std::vector<Vertex>& v)
{
	resource_mesh->SetVertices(v);
}

void ComponentMesh::SetIndices(const std::vector<uint>& i)
{
	resource_mesh->SetIndices(i);
}

void ComponentMesh::SetPath(const char * str)
{
	resource_mesh->SetOwnFile(str);
}

void ComponentMesh::SetDrawMaterial(ComponentMaterial * mat)
{
	draw_material = mat;
}

void ComponentMesh::SetResourceMesh(ResourceMesh * res, bool ad_ref)
{
	resource_mesh = res;
	if(ad_ref)resource_mesh->AddReference();
}

// Functionality ================================
void ComponentMesh::SetupMesh()
{
	resource_mesh->SetupMesh();
}

bool ComponentMesh::MeshResourceIsNull() const
{
	return bool(resource_mesh == nullptr);
}

void ComponentMesh::BlitComponentInspector()
{
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Mesh");

	if (resource_mesh == nullptr)ImGui::Text("NULL MESH RESOURCE");

	else
	{
		//Show mesh Tris & Vertex
		ImGui::Text("Tris: %i", resource_mesh->GetNumTris());
		ImGui::SameLine();
		ImGui::Text("Vertex: %i", resource_mesh->GetNumVertex());
	}
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
	
	//Insert the resource mesh ID
	comp_data.InsertInt("resource_mesh_id", resource_mesh->GetID());

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
	SetResourceMesh((ResourceMesh*)App->res_manager->Find(data.GetInt("resource_mesh_id")));
		
	//Get draw material id
	uint material_id = data.GetInt("draw_material_id");
	if(material_id != 0)links.push_back(std::pair<Component*, uint>(this, material_id));

	return ret;
}

void ComponentMesh::LinkComponent(const Component * target)
{
	draw_material = (ComponentMaterial*)target;
}

void ComponentMesh::UnLinkComponent()
{
	ComponentMeshRenderer* rend = (ComponentMeshRenderer*)parent->FindComponent(COMPONENT_TYPE::COMP_MESH_RENDERER);
	if (rend != nullptr)rend->SetTargetMesh(nullptr);
}
