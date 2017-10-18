#include "ComponentTransform.h"
#include "GameObject.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

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

bool ComponentTransform::Update()
{
	if (has_been_modified)
	{
		UpdateTransform();
	}

	return true;
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
	transform_matrix.Decompose(position, rotation_quaternion, scale);
	rotation_euler_angles = (rotation_quaternion.ToEulerXYZ() * RADTODEG);
}

void ComponentTransform::SetTransformation(math::float4x4 trans)
{
	transform_matrix = trans;

	transform_matrix.Decompose(position, rotation_quaternion, scale);
	rotation_euler_angles = rotation_quaternion.ToEulerXYZ();
}

// Get Methods ==================================
math::float3 ComponentTransform::GetPosition() const
{
	return position;
}

math::float3 ComponentTransform::GetRotationEuler() const
{
	return rotation_euler_angles;
}

math::Quat ComponentTransform::GetRotationQuat() const
{
	return rotation_quaternion;
}

math::float3 ComponentTransform::GetScale() const
{
	return scale;
}

const float* ComponentTransform::GetTransformMatrixRows() const
{
	return transform_matrix.ptr();
}

const float* ComponentTransform::GetTransformMatrixColumns() const
{
	return transform_matrix.Transposed().ptr();
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

}

void ComponentTransform::UpdateTransform()
{
	rotation_quaternion = math::Quat::FromEulerXYZ(rotation_euler_angles.x * DEGTORAD, rotation_euler_angles.y  * DEGTORAD, rotation_euler_angles.z  * DEGTORAD);

	transform_matrix = math::float4x4::FromQuat(rotation_quaternion);
	transform_matrix = math::float4x4::Scale(scale, math::float3(0, 0, 0)) * transform_matrix;
	transform_matrix.SetTranslatePart(position.x, position.y, position.z);
	has_been_modified = false;
}

void ComponentTransform::SetMatrixToDraw()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	ComponentTransform lol;
	lol.SetTransformation(transform_matrix);

	if (parent->GetParent())
	{
		ComponentTransform* parent_transform = (ComponentTransform*)parent->GetParent()->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
		if (parent_transform)
		{
			//lol.SetTransformation(parent_transform->transform_matrix * transform_matrix);
			glMultMatrixf(parent_transform->GetTransformMatrixColumns());
		}
	}

	glMultMatrixf(GetTransformMatrixColumns());
	

}

void ComponentTransform::QuitMatrixToDraw()
{
	glPopMatrix();

	if (parent)
		glPopMatrix();
}
