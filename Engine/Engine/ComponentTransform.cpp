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

	if (!(parent->GetParent()->IsRoot()))
	{
		ComponentTransform* tmp = ((ComponentTransform*)parent->GetParent()->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION));
		inherited_transform = tmp->inherited_transform * transform_matrix;
		inherited_position = tmp->position;
	}

	if (draw_axis && parent->IsSelectedObject())
	{
		DrawOrientationAxis();
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

math::float4x4 ComponentTransform::GetTransform() const
{
	return transform_matrix;
}

math::float4x4 ComponentTransform::GetInheritedTransform() const
{
	return inherited_transform;
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
	ImGui::SameLine();
	ImGui::Checkbox("Draw Axis##transform", &draw_axis);

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
	if (ImGui::DragFloat("X##rotation", &rotation_euler_angles.x, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;
	ImGui::SameLine();
	if (ImGui::DragFloat("Y##rotation", &rotation_euler_angles.y, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;
	ImGui::SameLine();
	if (ImGui::DragFloat("Z##rotation", &rotation_euler_angles.z, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;

	//Transform scale
	ImGui::Text("Scale    ");
	ImGui::SameLine();
	if (ImGui::DragFloat("X##scale", &scale.x, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;
	ImGui::SameLine();
	if (ImGui::DragFloat("Y##scale", &scale.y, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;
	ImGui::SameLine();
	if (ImGui::DragFloat("Z##scale", &scale.z, 0.5f, 0.0f, 0.0f, "%.2f")) has_been_modified = true;
	ImGui::PopItemWidth();

	ImGui::Text("Normalized Scale");
	float norm = (scale.x + scale.y + scale.z) / 3.0f;
	if (ImGui::DragFloat("Normalized Scale", &norm, 0.2f, 0.1f))
	{
		if (norm < 0.01f)norm = 0.01f;
		scale.SetFromScalar(norm);
		has_been_modified = true;
	}
}

void ComponentTransform::UpdateTransform()
{
	rotation_quaternion = math::Quat::FromEulerXYZ(rotation_euler_angles.x * DEGTORAD, rotation_euler_angles.y  * DEGTORAD, rotation_euler_angles.z  * DEGTORAD);

	transform_matrix = math::float4x4::FromQuat(rotation_quaternion);
	transform_matrix = math::float4x4::Scale(scale, math::float3(0, 0, 0)) * transform_matrix;
	transform_matrix.SetTranslatePart(position.x, position.y, position.z);

	//If its parent is scene update inherited matrix

	if (parent->GetParent()->IsRoot())
	{
		inherited_transform = transform_matrix;
		inherited_position = position;
		parent->GetBoundingBox()->Scale(parent->GetBoundingBox()->CenterPoint(),scale);
	}


	has_been_modified = false;
}

void ComponentTransform::SetMatrixToDraw()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMultMatrixf(inherited_transform.Transposed().ptr());
	

}

void ComponentTransform::QuitMatrixToDraw()
{
	glPopMatrix();
}

void ComponentTransform::DrawOrientationAxis() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	//AXIS X
	math::float3 axis = inherited_transform.Col3(0).Normalized();
	math::float3 pos;

	if (!(parent->GetParent()->IsRoot()))
	{
		pos = inherited_position + position;
	}
	else
	{
		pos = position;
	}
	
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(pos.x, pos.y, pos.z); glVertex3f(axis.x + pos.x, axis.y + pos.y, axis.z + pos.z);

	//AXIS Y
	axis = inherited_transform.Col3(1).Normalized();
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(pos.x, pos.y, pos.z); glVertex3f(axis.x + pos.x, axis.y + pos.y, axis.z + pos.z);

	//AXIS Z
	axis = inherited_transform.Col3(2).Normalized();
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(pos.x, pos.y, pos.z); glVertex3f(axis.x + pos.x, axis.y + pos.y, axis.z + pos.z);

	glEnd();

	glLineWidth(1.0f);
}
