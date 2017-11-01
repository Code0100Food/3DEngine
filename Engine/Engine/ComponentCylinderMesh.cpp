#include "ComponentCylinderMesh.h"

#include "ComponentMaterial.h"
#include "Serializer.h"

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
