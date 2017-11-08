#include "ComponentCylinderMesh.h"

#include "Application.h"
#include "ComponentMaterial.h"
#include "Serializer.h"
#include "CylinderGenerator.h"
#include "GameObject.h"
#include "ResourceMesh.h"
#include "ResourcesManager.h"

// Constructors =================================
ComponentCylinderMesh::ComponentCylinderMesh()
{
	type = COMP_CYLINDER_MESH;
}

ComponentCylinderMesh::ComponentCylinderMesh(const ComponentCylinderMesh & cpy) :ComponentPrimitiveMesh(cpy), geometry(cpy.geometry)
{

}

// Destructors ==================================
ComponentCylinderMesh::~ComponentCylinderMesh()
{

}

// Set Methods ==================================
void ComponentCylinderMesh::SetGeometry(const math::Cylinder & geom)
{
	geometry = geom;
}

// Functionality ================================
void ComponentCylinderMesh::BlitComponentInspector()
{
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Cylinder Mesh");

	if (resource_mesh == nullptr)ImGui::Text("NULL MESH RESOURCE");

	else
	{
		//Show mesh Tris & Vertex
		ImGui::Text("Tris: %i", resource_mesh->GetNumTris());
		ImGui::SameLine();
		ImGui::Text("Vertex: %i", resource_mesh->GetNumVertex());
	}
}

bool ComponentCylinderMesh::Save(Serializer & array_root) const
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
	//Insert top
	Serializer top_array = comp_data.InsertArray("top");
	for (uint k = 0; k < 3; k++)top_array.InsertArrayFloat(geometry.l.b.ptr()[k]);
	//Insert bottom
	Serializer bottom_array = comp_data.InsertArray("bottom");
	for (uint k = 0; k < 3; k++)bottom_array.InsertArrayFloat(geometry.l.a.ptr()[k]);
	//Insert radius
	comp_data.InsertFloat("radius", geometry.r);

	//Insert draw material id
	if (draw_material != nullptr)ret = comp_data.InsertInt("draw_material_id", draw_material->GetID());

	//Save the built data in the components array
	ret = array_root.InsertArrayElement(comp_data);

	return ret;
}

bool ComponentCylinderMesh::Load(Serializer & data, std::vector<std::pair<Component*, uint>>& links)
{
	bool ret = true;

	//Get component id
	id = data.GetInt("id");
	//Get actived
	actived = data.GetBool("actived");

	//Get primitive type
	primitive_type = StrToPrimitiveType(data.GetString("primitive_type"));

	//Get draw material id
	uint material_id = data.GetInt("draw_material_id");
	if (material_id != 0)links.push_back(std::pair<Component*, uint>(this, material_id));

	//Get geometry data
	//Get top
	Serializer top_array = data.GetArray("top");
	for (uint k = 0; k < 3; k++)geometry.l.b.ptr()[k] = top_array.GetArrayFloat(k);
	//Get bottom
	Serializer bottom_array = data.GetArray("bottom");
	for (uint k = 0; k < 3; k++)geometry.l.a.ptr()[k] = bottom_array.GetArrayFloat(k);
	//Get radius
	geometry.r = data.GetFloat("radius");

	//Build the mesh with the loaded values
	resource_mesh = App->res_manager->GetPrimitiveResourceMesh(primitive_type);

	return ret;
}

void ComponentCylinderMesh::UnLinkComponent()
{
	ComponentMeshRenderer* rend = (ComponentMeshRenderer*)parent->FindComponent(COMPONENT_TYPE::COMP_MESH_RENDERER);
	if (rend != nullptr)rend->SetTargetMesh(nullptr);
}
