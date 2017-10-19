#include "ComponentCamera.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"

// Constructors =================================
ComponentCamera::ComponentCamera()
{
	
}

ComponentCamera::ComponentCamera(const ComponentCamera & cpy) :Component(cpy)
{

}

// Destructors ==================================
ComponentCamera::~ComponentCamera()
{

}

bool ComponentCamera::Start()
{
	frustum.nearPlaneDistance = 1;
	frustum.farPlaneDistance = 15;

	UpdateFrustrum();
	
	//Set frustum looking to GameObject Z axis
	const ComponentTransform* parent_transform = (ComponentTransform*)this->parent->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);

	frustum.type = math::PerspectiveFrustum;

	frustum.pos = parent_transform->GetPosition();
	frustum.front = math::float3(0, 0, 1);
	frustum.up = math::float3(0, 1, 0);

	frustum.orthographicHeight = 5;
	frustum.orthographicWidth = 2;

	frustum.verticalFov = 5;
	frustum.horizontalFov = 8;

	return true;
}

bool ComponentCamera::Update()
{
	UpdateFrustumTransform();
	DrawFrustum();

	if (frustum_culling)
	{
		ApplyFrustum(App->scene->GetRoot());
	}

	return true;
}

// Functionality ================================
void ComponentCamera::SetTransform(const ComponentTransform * trans)
{

}

void ComponentCamera::UpdateFrustrum()
{
	//Set frustum looking to GameObject Z axis
	const ComponentTransform* parent_transform = (ComponentTransform*)this->parent->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
	
	frustum.type = math::PerspectiveFrustum;

	frustum.pos = math::float3(0, 0, 0);
	frustum.front = math::float3(0, 0, 1);
	frustum.up = math::float3(0, 1, 0);

	frustum.orthographicHeight = 5;
	frustum.orthographicWidth = 2;

	frustum.verticalFov = 5;
	frustum.horizontalFov = 8;
}

void ComponentCamera::UpdateFrustumTransform()
{
	const ComponentTransform* parent_transform = (ComponentTransform*)this->parent->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);

	//Z axis of the transform
	frustum.front = parent_transform->transform_matrix.Row3(2);

	//Y axis of the transform
	frustum.up = parent_transform->transform_matrix.Row3(1);

	frustum.pos = parent_transform->GetPosition();
}

void ComponentCamera::DrawFrustum() const
{
	App->renderer3D->DisableGLRenderFlags();
	
	float color[4] = { 0.7f, 0.5f, 0.5f, 1 };
	frustum.Draw(3.0f, color);

	App->renderer3D->EnableGLRenderFlags();
}

void ComponentCamera::ApplyFrustum(GameObject * target)
{
	if (target == nullptr)return;

	std::vector<GameObject*> childs = *target->GetChilds();
	uint size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		ApplyFrustum(childs[k]);
	}
	
	if (target != parent && !frustum.Contact(*target->GetBoundingBox()))
	{
		target->SetActiveState(false);
	}
	else target->SetActiveState(true);
}

void ComponentCamera::UnApplyFrustum(GameObject * target)
{
	if (target == nullptr)return;

	std::vector<GameObject*> childs = *target->GetChilds();
	uint size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		UnApplyFrustum(childs[k]);
	}

	target->SetActiveState(true);
}

void ComponentCamera::BlitComponentInspector()
{
	ImGui::Separator();

	ImGui::Checkbox("##camera_comp", &actived);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Camera");

	//Culling
	if (ImGui::Checkbox("Frustum Culling", &frustum_culling))
	{
		if (!frustum_culling)
		{
			UnApplyFrustum(App->scene->GetRoot());
		}
	}

	//Camera frustum variables

	//Front Vec
	float f_vec[3] = { frustum.front.x ,frustum.front.y,frustum.front.z };
	ImGui::DragFloat3("Front Vec", f_vec);
	frustum.front = { f_vec[0],f_vec[1],f_vec[2] };

	//Up Vec
	float u_vec[3] = { frustum.up.x ,frustum.up.y,frustum.up.z };
	ImGui::DragFloat3("Up Vec", u_vec);
	frustum.up = { u_vec[0],u_vec[1],u_vec[2] };

	//Near plane dist
	ImGui::DragFloat("Near Plane Dist", &frustum.nearPlaneDistance, 0.2f, 0.01f, 50, "%.2f");

	//Far plane dist
	ImGui::DragFloat("Far Plane Dist", &frustum.farPlaneDistance, 0.2f, 0.1f, 50, "%.2f");

	//Ortographic Height
	ImGui::DragFloat("Ortographic Height", &frustum.orthographicHeight, 0.05, 0.0, 2);

	//Ortographic Width
	ImGui::DragFloat("Ortographic Width", &frustum.orthographicWidth, 0.05, 0.0, 2);

}
