#include "ComponentCubeMesh.h"

#include "Application.h"
#include "ComponentMaterial.h"
#include "Serializer.h"
#include "CubeGenerator.h"
#include "GameObject.h"
#include "ResourceMesh.h"
#include "ResourcesManager.h"

// Constructors =================================
ComponentCubeMesh::ComponentCubeMesh()
{
	type = COMP_CUBE_MESH;
}

ComponentCubeMesh::ComponentCubeMesh(const ComponentCubeMesh & cpy) :ComponentPrimitiveMesh(cpy), geometry(cpy.geometry)
{

}

// Destructors ==================================
ComponentCubeMesh::~ComponentCubeMesh()
{

}

// Set Methods ==================================
void ComponentCubeMesh::SetGeometry(const math::AABB & geom)
{
	geometry = geom;
}

// Functionality ================================
void ComponentCubeMesh::BlitComponentInspector()
{
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Cube Mesh");

	if (resource_mesh == nullptr)ImGui::Text("NULL MESH RESOURCE");

	else
	{
		//Show mesh Tris & Vertex
		ImGui::Text("Tris: %i", resource_mesh->GetNumTris());
		ImGui::SameLine();
		ImGui::Text("Vertex: %i", resource_mesh->GetNumVertex());
	}
}

bool ComponentCubeMesh::Save(Serializer & array_root) const
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

	//Insert primitive type
	comp_data.InsertString("primitive_type", PrimitiveTypeToStr(primitive_type));

	//Insert geometry data
	//Insert min point
	Serializer min_point_array = comp_data.InsertArray("min_point");
	for (uint k = 0; k < 3; k++)min_point_array.InsertArrayFloat(geometry.minPoint.ptr()[k]);
	//Insert max point
	Serializer max_point_array = comp_data.InsertArray("max_point");
	for (uint k = 0; k < 3; k++)max_point_array.InsertArrayFloat(geometry.maxPoint.ptr()[k]);

	//Insert draw material id
	if (draw_material != nullptr)ret = comp_data.InsertInt("draw_material_id", draw_material->GetID());

	//Save the built data in the components array
	ret = array_root.InsertArrayElement(comp_data);

	return ret;
}

bool ComponentCubeMesh::Load(Serializer & data, std::vector<std::pair<Component*, uint>>& links)
{
	bool ret = true;

	//Get component id
	id = data.GetInt("id");
	//Get actived
	actived = data.GetBool("actived");

	//Get primitive type
	primitive_type = StrToPrimitiveType(data.GetString("primitive_type"));

	//Get geometry data
	//Get min point
	Serializer min_point_array = data.GetArray("min_point");
	for (uint k = 0; k < 3; k++)geometry.minPoint.ptr()[k] = min_point_array.GetArrayFloat(k);
	//Get max point
	Serializer max_point_array = data.GetArray("max_point");
	for (uint k = 0; k < 3; k++)geometry.maxPoint.ptr()[k] = max_point_array.GetArrayFloat(k);

	//Get draw material id
	uint material_id = data.GetInt("draw_material_id");
	if (material_id != 0)links.push_back(std::pair<Component*, uint>(this, material_id));

	//Build the mesh with the loaded values
	resource_mesh = App->res_manager->GetPrimitiveResourceMesh(primitive_type);

	return ret;
}

void ComponentCubeMesh::UnLinkComponent()
{
	ComponentMeshRenderer* rend = (ComponentMeshRenderer*)parent->FindComponent(COMPONENT_TYPE::COMP_MESH_RENDERER);
	if (rend != nullptr)rend->SetTargetMesh(nullptr);
}
