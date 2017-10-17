#include "ComponentCamera.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "Application.h"
#include "ModuleScene.h"

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

	frustum.pos = math::float3(0, 0, 0);
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
	DrawFrustum();

	if (frustum_culling)
	{
		//GameObject* root = App->scene->GetRoot();
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

void ComponentCamera::DrawFrustum() const
{
	std::vector<math::float3> bb_vertex;
	bb_vertex.reserve(8);
	frustum.GetCornerPoints(bb_vertex.data());

	//Draw bounding box
	glLineWidth(3.f);
	glColor4f(0.2f, 0.2f, 0.2f, 1);

	glBegin(GL_LINES);

	for (uint k = 0; k < 4; k++)
	{
		glVertex3f(bb_vertex.data()[k + 4].x, bb_vertex.data()[k + 4].y, bb_vertex.data()[k + 4].z);
		glVertex3f(bb_vertex.data()[k].x, bb_vertex.data()[k].y, bb_vertex.data()[k].z);
	}

	for (uint k = 0; k <= 4; k += 4)
	{
		glVertex3f(bb_vertex.data()[k].x, bb_vertex.data()[k].y, bb_vertex.data()[k].z);
		glVertex3f(bb_vertex.data()[k + 1].x, bb_vertex.data()[k + 1].y, bb_vertex.data()[k + 1].z);

		glVertex3f(bb_vertex.data()[k + 2].x, bb_vertex.data()[k + 2].y, bb_vertex.data()[k + 2].z);
		glVertex3f(bb_vertex.data()[k + 3].x, bb_vertex.data()[k + 3].y, bb_vertex.data()[k + 3].z);

		glVertex3f(bb_vertex.data()[k].x, bb_vertex.data()[k].y, bb_vertex.data()[k].z);
		glVertex3f(bb_vertex.data()[k + 2].x, bb_vertex.data()[k + 2].y, bb_vertex.data()[k + 2].z);

		glVertex3f(bb_vertex.data()[k + 1].x, bb_vertex.data()[k + 1].y, bb_vertex.data()[k + 1].z);
		glVertex3f(bb_vertex.data()[k + 3].x, bb_vertex.data()[k + 3].y, bb_vertex.data()[k + 3].z);
	}
	glEnd();
}

void ComponentCamera::BlitComponentInspector()
{
	ImGui::Separator();

	ImGui::Checkbox("##camera_comp", &actived);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Camera");

	//Culling
	ImGui::Checkbox("Frustum Culling", &frustum_culling);

	//Camera frustum variables
	ImGui::SliderFloat("Near Plane Dist", &frustum.nearPlaneDistance, 0.01f, 50, "%.2f", 0.2f);
	ImGui::SliderFloat("Far Plane Dist", &frustum.farPlaneDistance, 0.1f, 50, "%.2f", 0.2f);

	//Temporal for test
	float pos[3] = { frustum.pos.x ,frustum.pos.y,frustum.pos.z };
	ImGui::SliderFloat3("Pos TEMP", pos, -10, 10, "%.1f", 0.3f);
	frustum.pos = { pos[0],pos[1],pos[2] };
}
