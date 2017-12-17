#include "ComponentCamera.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ModuleImgui.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include <queue>
#include "Serializer.h"

// Constructors =================================
ComponentCamera::ComponentCamera() : Component(COMP_CAMERA)
{
	
}

ComponentCamera::ComponentCamera(const ComponentCamera & cpy) : Component(cpy), frustum(cpy.frustum)
{
	App->renderer3D->AddGameCamera(this);
}

// Destructors ==================================
ComponentCamera::~ComponentCamera()
{
	App->renderer3D->RemoveGameCamera(this);
}

// Game Loop ====================================
bool ComponentCamera::Start()
{
	frustum.type = math::PerspectiveFrustum;

	frustum.nearPlaneDistance = 0.01;
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

bool ComponentCamera::Update(float dt)
{
	UpdateFrustumTransform();

	if (draw_frustrum)
	{
		DrawFrustum();
	}
	
	if (frustum_culling)
	{
		ApplyDinamicFrustum();
		ApplyStaticFrustum();
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

void ComponentCamera::SetVerticalFov(float angle_in_deg, float aspect_ratio)
{
	frustum.verticalFov = angle_in_deg * DEGTORAD;
	frustum.horizontalFov = (2 * math::Atan(math::Tan(frustum.verticalFov / 2) * aspect_ratio));
}

void ComponentCamera::SetDrawFrustrum(bool active)
{
	draw_frustrum = active;
}

// Functionality ================================
void ComponentCamera::SetTransform(const ComponentTransform * trans)
{

}


void ComponentCamera::UpdateFrustumTransform()
{
	const ComponentTransform* parent_transform = (ComponentTransform*)this->parent->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);

	//Z axis of the transform
	frustum.front = parent_transform->GetTransform().Col3(2).Normalized();

	//Y axis of the transform
	frustum.up = parent_transform->GetTransform().Col3(1).Normalized();

	frustum.pos = parent_transform->GetPosition();
}

void ComponentCamera::DrawFrustum() const
{
	App->renderer3D->DisableGLRenderFlags();
	
	float color[4] = { 0.7f, 0.5f, 0.5f, 1 };
	frustum.Draw(3.0f, color);

	App->renderer3D->EnableGLRenderFlags();
}

void ComponentCamera::ApplyDinamicFrustum()
{
	GameObject * target = App->scene->GetRoot();

	if (target == nullptr)return;

	//Add Parent to the queue
	if(!target->GetStatic())remaining_childs.push(target);

	while (!remaining_childs.empty())
	{
		//Look if the front element is root node or camera parent or element is out the frustum
		if (remaining_childs.front() != parent && remaining_childs.front()->GetParent() != nullptr && frustum.VertexOutside(*remaining_childs.front()->GetBoundingBox()))
		{
			//if it is we will ignore its childs and set him to false
			remaining_childs.front()->SetHideState(true);
			remaining_childs.pop();
		}
		else
		{
			//else we will print it and check his childs
			remaining_childs.front()->SetHideState(false);

			for (std::vector<GameObject*>::iterator item = remaining_childs.front()->GetChilds()->begin(); item != remaining_childs.front()->GetChilds()->end(); item++)
			{
				if(!((GameObject*)*item)->GetStatic())remaining_childs.push((*item));
			}

			remaining_childs.pop();

		}
	}
}

void ComponentCamera::ApplyStaticFrustum()
{
	App->scene->CollectOctreeCandidates(frustum, remaining_childs);
	App->scene->HideStaticObjects();
	
	while (!remaining_childs.empty())
	{
		remaining_childs.front()->SetHideState(false);
		remaining_childs.pop();
	}
}

void ComponentCamera::UnApplyFrustum(GameObject * target)
{
	if (target == nullptr)return;

	remaining_childs.push(target);

	while (!remaining_childs.empty())
	{
		remaining_childs.front()->SetHideState(false);
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

bool ComponentCamera::Save(Serializer & array_root) const
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

	//Insert frustum data
	//Insert far plane dist
	ret = comp_data.InsertFloat("far_plane_dist", frustum.farPlaneDistance);
	//Insert near plane dist
	ret = comp_data.InsertFloat("near_plane_dist", frustum.nearPlaneDistance);
	//Insert up vector
	Serializer up_vec_array = comp_data.InsertArray("up_vec");
	for (uint k = 0; k < 3; k++)up_vec_array.InsertArrayFloat(frustum.up.ptr()[k]);
	//Insert front vector
	Serializer front_vec_array = comp_data.InsertArray("front");
	for (uint k = 0; k < 3; k++)front_vec_array.InsertArrayFloat(frustum.front.ptr()[k]);
	//Insert vertical fov
	comp_data.InsertFloat("vertical_fov", frustum.verticalFov);
	//Insert position
	Serializer pos_array = comp_data.InsertArray("position");
	for (uint k = 0; k < 3; k++)pos_array.InsertArrayFloat(frustum.pos.ptr()[k]);

	//Save the built data in the components array
	ret = array_root.InsertArrayElement(comp_data);

	return ret;
}

bool ComponentCamera::Load(Serializer & data, std::vector<std::pair<Component*, uint>>& links)
{
	bool ret = true;

	//Get component id
	id = data.GetInt("id");
	//Get actived
	actived = data.GetBool("actived");

	//Get frustum data
	//Get far plane dist
	frustum.farPlaneDistance = data.GetFloat("far_plane_dist");
	//Get near plane dist
	frustum.nearPlaneDistance = data.GetFloat("near_plane_dist");
	//Get up vector
	Serializer up_vec_array = data.GetArray("up_vec");
	for (uint k = 0; k < 3; k++)frustum.up.ptr()[k] = up_vec_array.GetArrayFloat(k);
	//Get front vector
	Serializer front_vec_array = data.GetArray("front");
	for (uint k = 0; k < 3; k++)frustum.front.ptr()[k] = front_vec_array.GetArrayFloat(k);
	//Get vertical fov
	frustum.verticalFov = data.GetFloat("vertical_fov");
	//Get position
	Serializer pos_array = data.GetArray("position");
	for (uint k = 0; k < 3; k++)frustum.pos.ptr()[k] = pos_array.GetArrayFloat(k);

	//Frustum default type
	frustum.type = math::FrustumType::PerspectiveFrustum;

	return ret;
}
