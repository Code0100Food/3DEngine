#include "ComponentTransform.h"

// Constructors =================================
ComponentTransform::ComponentTransform() : Component(COMP_TRANSFORMATION)
{

}

ComponentTransform::ComponentTransform(const ComponentTransform & cpy) : Component(cpy), position(cpy.position), scale(cpy.scale), rotation(cpy.rotation)
{

}

// Destructors ==================================
ComponentTransform::~ComponentTransform()
{

}

// Set Methods ==================================
void ComponentTransform::SetTransformation(aiMatrix4x4 trans)
{
	//trans.Decompose(scale, rotation, position);
	
	float values[16] = 
	{ 
		trans.a1, trans.a2, trans.a3, trans.a4,
		trans.b1, trans.b2, trans.b3, trans.b4,
		trans.c1, trans.c2, trans.c3, trans.c4,
		trans.d1, trans.d2, trans.d3, trans.d4 
	};

	transform_matrix.Set(values);
	transform_matrix.Decompose(position, rotation, scale);
	rotation_euler_angles = rotation.ToEulerXYZ();
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

	//Transform Position
	ImGui::Text("Position ");
	ImGui::SameLine();
	ImGui::PushItemWidth(50);
	ImGui::DragFloat("X", &position.x, 0.5f, 0.0f, 0.0f, "%.2f");
	ImGui::SameLine();
	ImGui::DragFloat("Y", &position.y, 0.5f, 0.0f, 0.0f, "%.2f");
	ImGui::PushItemWidth(50);
	ImGui::SameLine();
	ImGui::DragFloat("Z", &position.z, 0.5f, 0.0f, 0.0f, "%.2f");


	//Transform rotation
	ImGui::Text("Rotation ");
	ImGui::SameLine();
	ImGui::PushItemWidth(50);
	ImGui::DragFloat("X", &rotation_euler_angles.x, 0.5f, 0.0f, 0.0f, "%.2f");
	ImGui::SameLine();
	ImGui::DragFloat("Y", &rotation_euler_angles.y, 0.5f, 0.0f, 0.0f, "%.2f");
	ImGui::PushItemWidth(50);
	ImGui::SameLine();
	ImGui::DragFloat("Z", &rotation_euler_angles.z, 0.5f, 0.0f, 0.0f, "%.2f");

	//Transform rotation
	ImGui::Text("Scale    ");
	ImGui::SameLine();
	ImGui::PushItemWidth(50);
	ImGui::DragFloat("X", &scale.x, 0.5f, 0.0f, 0.0f, "%.2f");
	ImGui::SameLine();
	ImGui::DragFloat("Y", &scale.y, 0.5f, 0.0f, 0.0f, "%.2f");
	ImGui::PushItemWidth(50);
	ImGui::SameLine();
	ImGui::DragFloat("Z", &scale.z, 0.5f, 0.0f, 0.0f, "%.2f");
}
