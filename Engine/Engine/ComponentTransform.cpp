#include "ComponentTransform.h"

// Constructors =================================
ComponentTransform::ComponentTransform() : Component(COMP_TRANSFORMATION)
{

}

ComponentTransform::ComponentTransform(const ComponentTransform & cpy) : Component(cpy), position(cpy.position), scale(cpy.scale), rotation_euler_angles(cpy.rotation_euler_angles), transform_matrix(cpy.transform_matrix)
{

}

// Destructors ==================================
ComponentTransform::~ComponentTransform()
{

}

// Set Methods ==================================
void ComponentTransform::SetTransformation(aiMatrix4x4 trans)
{
	//Set the mathgeolib matrix from assim matrix
	float values[16] =
	{
		trans.a1, trans.a2, trans.a3, trans.a4,
		trans.b1, trans.b2, trans.b3, trans.b4,
		trans.c1, trans.c2, trans.c3, trans.c4,
		trans.d1, trans.d2, trans.d3, trans.d4
	};

	transform_matrix.Set(values);

	//Set the variables that will be shown in the UI
	math::Quat	rotation;
	transform_matrix.Decompose(position, rotation, scale);
	rotation_euler_angles = (rotation.ToEulerXYZ() * RADTODEG);
}

// Get Methods ==================================
math::float3 ComponentTransform::GetPosition() const
{
	return position;
}


// Functionality ================================
void ComponentTransform::BlitComponentInspector()
{
	ImGui::Separator();

	ImGui::Checkbox("##transform_comp", &actived);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Transform");

	//Modified
	bool has_been_modified = false;

	//Transform Position
	ImGui::Text("Position ");
	ImGui::SameLine();
	ImGui::PushItemWidth(50);
	if (ImGui::DragFloat("X##position", &position.x, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;
	ImGui::SameLine();
	if (ImGui::DragFloat("Y##position", &position.y, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;
	ImGui::SameLine();
	if (ImGui::DragFloat("Z##position", &position.z, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;

	//Transform rotation
	bool has_rotate = false;
	ImGui::Text("Rotation ");
	ImGui::SameLine();
	ImGui::PushItemWidth(50);
	if (ImGui::DragFloat("X##rotation", &rotation_euler_angles.x, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;
	ImGui::SameLine();
	if (ImGui::DragFloat("Y##rotation", &rotation_euler_angles.y, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;
	ImGui::SameLine();
	if (ImGui::DragFloat("Z##rotation", &rotation_euler_angles.z, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;

	//Transform rotation
	bool has_scale = false;
	ImGui::Text("Scale    ");
	ImGui::SameLine();
	if (ImGui::DragFloat("X##scale", &scale.x, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;
	ImGui::SameLine();
	if (ImGui::DragFloat("Y##scale", &scale.y, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;
	ImGui::SameLine();
	if (ImGui::DragFloat("Z##scale", &scale.z, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;
	ImGui::PopItemWidth();

	if (has_been_modified)
	{
		transform_matrix = transform_matrix.FromEulerXYZ(rotation_euler_angles.x * DEGTORAD, rotation_euler_angles.y  * DEGTORAD, rotation_euler_angles.z  * DEGTORAD);
		transform_matrix = transform_matrix * transform_matrix.Scale(scale, math::float3(0, 0, 0));
		transform_matrix.SetTranslatePart(position.x, position.y, position.z);
	}
}
