#include "ComponentSphereMesh.h"

#include "Application.h"
#include "ComponentMaterial.h"
#include "Serializer.h"
#include "SphereGenerator.h"
#include "GameObject.h"
#include "ResourceMesh.h"
#include "ResourcesManager.h"

// Constructors =================================
ComponentSphereMesh::ComponentSphereMesh()
{
	type = COMP_SPHERE_MESH;
}

ComponentSphereMesh::ComponentSphereMesh(const ComponentSphereMesh & cpy) :ComponentPrimitiveMesh(cpy), geometry(cpy.geometry)
{

}

// Destructors ==================================
ComponentSphereMesh::~ComponentSphereMesh()
{

}

// Set Methods ==================================
void ComponentSphereMesh::SetGeometry(const math::Sphere & geom)
{
	geometry = geom;
}

// Functionality ================================
void ComponentSphereMesh::BlitComponentInspector()
{
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Sphere Mesh");

	if (resource_mesh == nullptr)ImGui::Text("NULL MESH RESOURCE");

	else
	{
		//Show mesh Tris & Vertex
		ImGui::Text("Tris: %i", resource_mesh->GetNumTris());
		ImGui::SameLine();
		ImGui::Text("Vertex: %i", resource_mesh->GetNumVertex());
	}
}

bool ComponentSphereMesh::Save(Serializer & array_root) const
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
	//Insert radius
	comp_data.InsertFloat("radius",geometry.r);

	//Insert draw material id
	if (draw_material != nullptr)ret = comp_data.InsertInt("draw_material_id", draw_material->GetID());

	//Save the built data in the components array
	ret = array_root.InsertArrayElement(comp_data);

	return ret;
}

bool ComponentSphereMesh::Load(Serializer & data, std::vector<std::pair<Component*, uint>>& links)
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
	//Set position
	geometry.pos = math::float3(0, 0, 0);
	//Get radius
	geometry.r = data.GetFloat("radius");
	
	//Build the mesh with the loaded values
	resource_mesh = App->res_manager->GetPrimitiveResourceMesh(primitive_type);
	resource_mesh->AddReference();

	return ret;
}

void ComponentSphereMesh::UnLinkComponent()
{
	ComponentMeshRenderer* rend = (ComponentMeshRenderer*)parent->FindComponent(COMPONENT_TYPE::COMP_MESH_RENDERER);
	if(rend != nullptr)rend->SetTargetMesh(nullptr);
}
