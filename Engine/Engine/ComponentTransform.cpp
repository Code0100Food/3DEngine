#include "ComponentTransform.h"
#include "GameObject.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "Serializer.h"


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

// Game Loop ====================================
bool ComponentTransform::Update(float dt)
{
	if (draw_axis && parent->IsSelectedObject())
	{
		DrawOrientationAxis();
	}

	if (has_been_modified)
	{
		UpdateTransform();
	}

	if (!parent->GetParent()->IsRoot())
	{
		ComponentTransform* tmp = ((ComponentTransform*)parent->GetParent()->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION));
		inherited_transform = transform_matrix.Transposed() * tmp->inherited_transform;
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

	//Set the inherited transform
	if (!(parent->GetParent()->IsRoot()))
	{
		ComponentTransform* tmp = ((ComponentTransform*)parent->GetParent()->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION));
		inherited_transform = tmp->inherited_transform * transform_matrix.Transposed();
	}
	else
	{
		inherited_transform = transform_matrix.Transposed();
	}
}

void ComponentTransform::SetTransformation(math::float4x4 trans)
{
	transform_matrix = trans;

	transform_matrix.Decompose(position, rotation_quaternion, scale);
	rotation_euler_angles = rotation_quaternion.ToEulerXYZ();

	//Set the inherited transform
	if (!(parent->GetParent()->IsRoot()))
	{
		ComponentTransform* tmp = ((ComponentTransform*)parent->GetParent()->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION));
		inherited_transform = tmp->inherited_transform * transform_matrix.Transposed();
	}
	else
	{
		inherited_transform = transform_matrix.Transposed();
	}
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
		inherited_transform = transform_matrix.Transposed();
		parent->GetBoundingBox()->Scale(parent->GetBoundingBox()->CenterPoint(),scale);
	}

	has_been_modified = false;
}

void ComponentTransform::SetMatrixToDraw()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMultMatrixf(inherited_transform.ptr());
}

void ComponentTransform::QuitMatrixToDraw()
{
	glPopMatrix();
}

void ComponentTransform::DrawOrientationAxis()
{
	
	if (App->renderer3D->GetGizmo())
	{
		App->renderer3D->GetGizmo()->SetEditMatrix(inherited_transform.ptr());
	}
	
}

void ComponentTransform::UpdateRotationPositionScale()
{

	if (parent->GetParent()->IsRoot())
	{
		transform_matrix = inherited_transform.Transposed();
		transform_matrix.Decompose(position, rotation_quaternion, scale);
		rotation_euler_angles = rotation_quaternion.ToEulerXYZ() * RADTODEG;
	}
}

bool ComponentTransform::Save(Serializer & array_root) const
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

	//Insert transform matrix
	Serializer trans_matrix_array = comp_data.InsertArray("transform_matrix");
	for (uint k = 0; k < 16; k++)trans_matrix_array.InsertArrayFloat(transform_matrix.ptr()[k]);
	//Insert position
	Serializer position_array = comp_data.InsertArray("position");
	for (uint k = 0; k < 3; k++)position_array.InsertArrayFloat(position.ptr()[k]);
	//Insert inherited transform matrix
	Serializer inherited_trans_array = comp_data.InsertArray("inherited_transform");
	for (uint k = 0; k < 16; k++)inherited_trans_array.InsertArrayFloat(inherited_transform.Transposed().ptr()[k]);

	//Insert scale
	Serializer scale_array = comp_data.InsertArray("scale");
	for (uint k = 0; k < 3; k++)scale_array.InsertArrayFloat(scale.ptr()[k]);
	//Insert euler angles
	Serializer euler_angles_array = comp_data.InsertArray("euler_angles");
	for (uint k = 0; k < 3; k++)euler_angles_array.InsertArrayFloat(rotation_euler_angles.ptr()[k]);

	//Save the built data in the components array
	ret = array_root.InsertArrayElement(comp_data);

	return ret;
}

bool ComponentTransform::Load(Serializer & data, std::vector<std::pair<Component*, uint>>& links)
{
	bool ret = true;

	//Get component id
	id = data.GetInt("id");
	//Get actived
	actived = data.GetBool("actived");

	//Get transform matrix
	Serializer trans_matrix_array = data.GetArray("transform_matrix");
	for (uint k = 0; k < 16; k++)transform_matrix.ptr()[k] = trans_matrix_array.GetArrayFloat(k);
	//Get position
	Serializer position_array = data.GetArray("position");
	for (uint k = 0; k < 3; k++)position.ptr()[k] = position_array.GetArrayFloat(k);
	//Get inherited transform matrix
	Serializer inherited_trans_array = data.GetArray("inherited_transform");
	for (uint k = 0; k < 16; k++)inherited_transform.ptr()[k] = inherited_trans_array.GetArrayFloat(k);
	//Get scale
	Serializer scale_array = data.GetArray("scale");
	for (uint k = 0; k < 3; k++)scale.ptr()[k] = scale_array.GetArrayFloat(k);
	//Get euler angles
	Serializer euler_angles_array = data.GetArray("euler_angles");
	for (uint k = 0; k < 3; k++)rotation_euler_angles.ptr()[k] = euler_angles_array.GetArrayFloat(k);
	
	//Calculate the resulting transform with the loaded data
	UpdateTransform();

	return ret;
}
