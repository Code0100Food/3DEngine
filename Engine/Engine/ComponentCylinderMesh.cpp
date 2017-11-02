#include "ComponentCylinderMesh.h"

#include "ComponentMaterial.h"
#include "Serializer.h"
#include "CylinderGenerator.h"

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

	//Show mesh Tris & Vertex
	ImGui::Text("Tris: %i", num_tris);
	ImGui::SameLine();
	ImGui::Text("Vertex: %i", num_vertex);
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

	//Insert geometry data
	//Insert top
	Serializer top_array = comp_data.InsertArray("top");
	for (uint k = 0; k < 3; k++)top_array.InsertArrayFloat(geometry.l.b.ptr()[k]);
	//Insert bottom
	Serializer bottom_array = comp_data.InsertArray("bottom");
	for (uint k = 0; k < 3; k++)bottom_array.InsertArrayFloat(geometry.l.a.ptr()[k]);
	//Insert radius
	comp_data.InsertFloat("radius", geometry.r);
	//Insert divisions
	comp_data.InsertInt("divisions", divisions);

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
	//Get divisions
	divisions = data.GetInt("divisions");

	//Build the mesh with the loaded values
	std::pair<std::vector<uint>, std::vector<Vertex>> mesh_data;
	CylinderGenerator cylinder;
	cylinder.SetGeometry(geometry);
	cylinder.SetDivisions(divisions);
	mesh_data = cylinder.Generate();
	SetIndices(mesh_data.first);
	SetVertices(mesh_data.second);
	SetupMesh();

	return ret;
}
