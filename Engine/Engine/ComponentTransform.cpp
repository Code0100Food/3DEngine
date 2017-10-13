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
	trans.Decompose(scale, rotation, position);
}

// Functionality ================================
void ComponentTransform::BlitComponentInspector()
{
	ImGui::Separator();

	ImGui::Checkbox("##transform_comp", &actived);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Transform");
	
	//Show mesh position
	ImGui::Text("Position	");
	ImGui::SameLine();
	ImGui::Text("X %.1f		", position.x);
	ImGui::SameLine();
	ImGui::Text("Y %.1f		", position.y);
	ImGui::SameLine();
	ImGui::Text("Z %.1f", position.z);

	float eX = atan2(-2 * (rotation.y*rotation.z - rotation.w*rotation.x), rotation.w*rotation.w - rotation.x*rotation.x - rotation.y*rotation.y + rotation.z*rotation.z);
	float eY = asin(2 * (rotation.x*rotation.z + rotation.w*rotation.y));
	float eZ = atan2(-2 * (rotation.x*rotation.y - rotation.w*rotation.z), rotation.w*rotation.w + rotation.x*rotation.x - rotation.y*rotation.y - rotation.z*rotation.z);

	//Show mesh rotation
	ImGui::Text("Rotation	");
	ImGui::SameLine();
	ImGui::Text("X %.1f		", eX);
	ImGui::SameLine();
	ImGui::Text("Y %.1f		", eY);
	ImGui::SameLine();
	ImGui::Text("Z %.1f", eZ);

	//Show mesh scale
	ImGui::Text("Scale	");
	ImGui::SameLine();
	ImGui::Text("X %.1f		", scale.x);
	ImGui::SameLine();
	ImGui::Text("Y %.1f		", scale.y);
	ImGui::SameLine();
	ImGui::Text("Z %.1f", scale.z);
}
