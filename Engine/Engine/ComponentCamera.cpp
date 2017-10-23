#include "ComponentCamera.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include <queue>

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
	App->renderer3D->RemoveGameCamera(this);
}

bool ComponentCamera::Start()
{
	frustum.type = math::PerspectiveFrustum;

	frustum.nearPlaneDistance = 1;
	frustum.farPlaneDistance = 15;
	
	//Set frustum looking to GameObject Z axis
	const ComponentTransform* parent_transform = (ComponentTransform*)this->parent->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);

	frustum.pos = parent_transform->GetPosition();
	frustum.front = parent_transform->GetTransform().Col3(2);
	frustum.up = parent_transform->GetTransform().Col3(1);

	frustum.verticalFov = 90 * DEGTORAD;
	frustum.horizontalFov = (2 * math::Atan(math::Tan(frustum.verticalFov / 2) * App->window->GetAspectRatio()));

	App->renderer3D->AddGameCamera(this);

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

const math::Frustum ComponentCamera::GetFrustum() const
{
	return frustum;
}

void ComponentCamera::SetIsMain(bool value)
{
	is_main = value;
}

// Functionality ================================
void ComponentCamera::SetTransform(const ComponentTransform * trans)
{

}


void ComponentCamera::UpdateFrustumTransform()
{
	const ComponentTransform* parent_transform = (ComponentTransform*)this->parent->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);

	//Z axis of the transform
	frustum.front = parent_transform->GetTransform().Col3(2);

	//Y axis of the transform
	frustum.up = parent_transform->GetTransform().Col3(1);

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

	//Add Parent to the queue
	remaining_childs.push(target);

	while (!remaining_childs.empty())
	{
		//Look if the front element is root node or camera parent or element is out the frustrum
		if (remaining_childs.front() != parent && remaining_childs.front()->GetParent() != nullptr && frustum.VertexOutside(remaining_childs.front()->GetTranslatedBoundingBox()))
		{
			//if it is we will ignore its childs and set him to false
			remaining_childs.front()->SetActiveState(false);
			remaining_childs.pop();
		}
		else
		{
			//else we will print it and check his childs
			remaining_childs.front()->SetActiveState(true);

			for (std::vector<GameObject*>::iterator item = remaining_childs.front()->GetChilds()->begin(); item != remaining_childs.front()->GetChilds()->end(); item++)
			{
				remaining_childs.push((*item));
			}

			remaining_childs.pop();

		}
	}
}

void ComponentCamera::UnApplyFrustum(GameObject * target)
{
	if (target == nullptr)return;

	remaining_childs.push(target);

	while (!remaining_childs.empty())
	{
		remaining_childs.front()->SetActiveState(true);
		for (std::vector<GameObject*>::iterator item = remaining_childs.front()->GetChilds()->begin(); item != remaining_childs.front()->GetChilds()->end(); item++)
		{
			remaining_childs.push((*item));
		}

		remaining_childs.pop();
	}

	
}

void ComponentCamera::BlitComponentInspector()
{
	ImGui::Separator();

	ImGui::Checkbox("##camera_comp", &actived);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Camera");
	ImGui::SameLine();
	if (ImGui::Checkbox("Main Camera##camera_comp", &is_main))
	{
		if (is_main)
		{
			App->renderer3D->SetMainCamera(this);
		}
	}


	//Culling
	if (ImGui::Checkbox("Frustum Culling", &frustum_culling))
	{
		if (!frustum_culling)
		{
			UnApplyFrustum(App->scene->GetRoot());
		}
	}

	//Camera frustum variables

	//Near plane dist
	ImGui::DragFloat("Near Plane Dist", &frustum.nearPlaneDistance, 0.2f, 0.01f, 50, "%.2f");

	//Far plane dist
	ImGui::DragFloat("Far Plane Dist", &frustum.farPlaneDistance, 0.2f, 0.1f, 50, "%.2f");

	//FOV
	float new_fov = frustum.verticalFov * RADTODEG;
	if (ImGui::DragFloat("Vertical FOV", &new_fov, 1.0, 1.0, 179))
	{
		frustum.verticalFov = new_fov * DEGTORAD;
		frustum.horizontalFov = (2 * math::Atan(math::Tan(frustum.verticalFov / 2) * App->window->GetAspectRatio()));
	}
}
