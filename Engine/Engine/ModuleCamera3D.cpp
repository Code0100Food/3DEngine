#include "Globals.h"

#include <math.h>

#include "ModuleWindow.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"
#include "ModuleAudio.h"
#include "FileSystem.h"
#include "ModuleInput.h"
#include "GeometryManager.h"
#include "ModuleScene.h"

// Constructors =================================
ModuleCamera3D::ModuleCamera3D(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled) : Module(_name, _id, _config_menu, _enabled)
{

}

// Destructors ==================================
ModuleCamera3D::~ModuleCamera3D()
{

}

// Game Loop ====================================
bool ModuleCamera3D::Awake(const JSON_Object * data_root)
{
	//Load camera location
	json_array_t* _array = json_object_get_array(data_root, "camera_location");
	editor_camera_frustrum.pos.x = json_array_get_number(_array, 0);
	editor_camera_frustrum.pos.y = json_array_get_number(_array, 1);
	editor_camera_frustrum.pos.z = json_array_get_number(_array, 2);
	//Load view vector
	_array = json_object_get_array(data_root, "focus_point");
	focus_point.x = json_array_get_number(_array, 0);
	focus_point.y = json_array_get_number(_array, 1);
	focus_point.z = json_array_get_number(_array, 2);
	//Load camera distance
	editor_camera_frustrum.farPlaneDistance = json_object_get_number(data_root, "far_plane_dist");
	//Load camera velocities
	camera_z_mov_vel = json_object_get_number(data_root, "camera_z_mov_vel");
	angle_to_rotate = json_object_get_number(data_root, "rot_around_angle");
	strafe_vel = json_object_get_number(data_root, "strafe_vel");
	wasd_vel = json_object_get_number(data_root, "wasd_vel");

	config_menu = true;

	return true;
}

bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	camera_z_mov_vel = 0.5f;

	editor_camera_frustrum.type = math::FrustumType::PerspectiveFrustum;

	editor_camera_frustrum.nearPlaneDistance = 1;

	editor_camera_frustrum.up = { 0,1,0 };
	editor_camera_frustrum.front = { 0,0,1 };

	editor_camera_frustrum.verticalFov = 60 * DEGTORAD;
	editor_camera_frustrum.horizontalFov = (2 * math::Atan(math::Tan(editor_camera_frustrum.verticalFov / 2) * App->window->GetAspectRatio()));

	return ret;
}

update_status ModuleCamera3D::Update(float dt)
{
	camera_dist = (focus_point - editor_camera_frustrum.pos).Length();

	//Camera only accept input if the mouse is over the viewport
	if (App->renderer3D->GetMouseOnWorkspace())
	{
		//Camera zoom in/out
		if (App->input->GetMouseZ() == 1)
		{
			editor_camera_frustrum.pos += editor_camera_frustrum.front * camera_z_mov_vel;
		}
		else if (App->input->GetMouseZ() == -1)
		{
			editor_camera_frustrum.pos -= editor_camera_frustrum.front * camera_z_mov_vel;
		}

		//Camera rotate around point
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
		{
			int dx = App->input->GetMouseXMotion();
			int dy = -App->input->GetMouseYMotion();

			if (dx != 0)
			{
				if (dx > 1)
					dx = 1;
				if(dx < -1)
					dx = -1;

				float DeltaX = (float)dx * rot_around_vel;

				
				rotate_quaternion.SetFromAxisAngle({ 0,1,0 }, angle_to_rotate * dx * DEGTORAD);
				editor_camera_frustrum.Transform(rotate_quaternion);

				if ((focus_point - editor_camera_frustrum.pos).Length() > camera_dist || (focus_point - editor_camera_frustrum.pos).Length() < camera_dist)
				{
					editor_camera_frustrum.pos = focus_point - (editor_camera_frustrum.front * camera_dist);
				}
	
			}

			if (dy != 0)
			{
				if (dy > 1)
					dy = 1;
				if (dy < -1)
					dy = -1;

				rotate_quaternion.SetFromAxisAngle(math::Cross(editor_camera_frustrum.front, editor_camera_frustrum.up), angle_to_rotate * dy * DEGTORAD);
				editor_camera_frustrum.Transform(rotate_quaternion);
				if ((focus_point - editor_camera_frustrum.pos).Length() > camera_dist || (focus_point - editor_camera_frustrum.pos).Length() < camera_dist)
				{
					editor_camera_frustrum.pos = focus_point - (editor_camera_frustrum.front * camera_dist);
				}
			}
		}

		//Camera stafe 
		if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
		{
			int dx = -App->input->GetMouseXMotion();
			int dy = -App->input->GetMouseYMotion();

			if (dx != 0)
			{
				float DeltaX = (float)dx * strafe_vel;

				editor_camera_frustrum.pos -= math::Cross(editor_camera_frustrum.front, editor_camera_frustrum.up) * DeltaX;
				focus_point -= math::Cross(editor_camera_frustrum.front, editor_camera_frustrum.up) * DeltaX;
			}
			if (dy != 0)
			{
				float DeltaY = (float)dy * strafe_vel;

				editor_camera_frustrum.pos -= editor_camera_frustrum.up * DeltaY;
				focus_point -= editor_camera_frustrum.up * DeltaY;
			}

		}

		//Camera wasd movement
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			{
				editor_camera_frustrum.pos -= editor_camera_frustrum.front * wasd_vel;
				focus_point -= editor_camera_frustrum.front * wasd_vel;

			}
			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			{
				editor_camera_frustrum.pos += editor_camera_frustrum.front * wasd_vel;
				focus_point += editor_camera_frustrum.front * wasd_vel;
			}
			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			{
				editor_camera_frustrum.pos += math::Cross(editor_camera_frustrum.front, editor_camera_frustrum.up) * wasd_vel;
				focus_point += math::Cross(editor_camera_frustrum.front, editor_camera_frustrum.up) * wasd_vel;
			}
			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			{
				editor_camera_frustrum.pos -= math::Cross(editor_camera_frustrum.front, editor_camera_frustrum.up) * wasd_vel;
				focus_point -= math::Cross(editor_camera_frustrum.front, editor_camera_frustrum.up) * wasd_vel;
			}
			if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT)
			{
				editor_camera_frustrum.pos += editor_camera_frustrum.up * wasd_vel;
				focus_point += editor_camera_frustrum.up * wasd_vel;
			}
			if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT)
			{
				editor_camera_frustrum.pos -= editor_camera_frustrum.up * wasd_vel;
				focus_point -= editor_camera_frustrum.up * wasd_vel;
			}
		}

		//Focus current object
		else if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		{
			GameObject* target = App->scene->GetSelectedGameObject();

			if (target != nullptr)
			{
				LookAtGameObject(target);
			}
			else LookAt({ 0,0,0 });
		}
	}

	//Look the vehicle body with the CameraLocation & the ViewVector & the Z_DIST defined
	App->camera->Look(focus_point, true);


	return UPDATE_CONTINUE;
}

bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

void ModuleCamera3D::BlitConfigInfo()
{
	//Camera location ui
	ImGui::InputFloat3("Camera Location", editor_camera_frustrum.pos.ptr(), 2);
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Change main camera location.");

	//View vector ui
	//ImGui::InputFloat3("View Vector", editor_camera_frustrum.front.ptr(), 2);
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Change main camera view vector.");

	if (ImGui::DragFloat("Far Plane Distance", &editor_camera_frustrum.farPlaneDistance, 1.0, NULL, NULL, "%.1f"))
	{
		App->audio->PlayFxForInput(FX_ID::SLICE_TICK_FX);
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Changes The frustrum far plane distance");

	ImGui::Separator();
	
	//Camera Velocities ui
	ImGui::Text("Sensibilities");
	ImGui::SliderFloat("Zoom Sensibility", &camera_z_mov_vel, 0.1f, 10.0f, "%.1f");
	ImGui::SliderFloat("Angle to Rotate around Sensibility", &angle_to_rotate, 0.1f, 360, "%.1f");
	ImGui::SliderFloat("Stafe Sensibility", &strafe_vel, 0.1f, 10.0f, "%.1f");
	ImGui::SliderFloat("WASD Sensibility", &wasd_vel, 0.1f, 10.0f, "%.1f");
}

void ModuleCamera3D::SaveConfigInfo(JSON_Object * data_root)
{
	//Save camera location
	json_array_t* _array = json_object_get_array(data_root, "camera_location");
	json_array_replace_number(_array, 0, editor_camera_frustrum.pos.x);
	json_array_replace_number(_array, 1, editor_camera_frustrum.pos.y);
	json_array_replace_number(_array, 2, editor_camera_frustrum.pos.z);
	//Save focus point
	_array = json_object_get_array(data_root, "focus_point");
	json_array_replace_number(_array, 0, focus_point.x);
	json_array_replace_number(_array, 1, focus_point.y);
	json_array_replace_number(_array, 2, focus_point.z);
	//Save far plane distance
	json_object_set_number(data_root, "far_plane_dist", editor_camera_frustrum.farPlaneDistance);
	//Save camera velocities
	json_object_set_number(data_root, "camera_z_mov_vel", camera_z_mov_vel);
	json_object_set_number(data_root, "rot_around_angle", angle_to_rotate);
	json_object_set_number(data_root, "strafe_vel", strafe_vel);
	json_object_set_number(data_root, "wasd_vel", wasd_vel);
}

// Functionality ================================

void ModuleCamera3D::Look(const math::float3& look_here, bool RotateAroundLookingPoint)
{
	editor_camera_frustrum.front = (look_here - editor_camera_frustrum.pos).Normalized();
	math::float3 tmp = math::Cross({ 0,1,0 }, editor_camera_frustrum.front).Normalized();
	editor_camera_frustrum.up = math::Cross(editor_camera_frustrum.front, tmp);
}

void ModuleCamera3D::LookAt( const math::float3 &Spot)
{
	focus_point = Spot;
	editor_camera_frustrum.pos = Spot -  (5 * editor_camera_frustrum.front);
}

void ModuleCamera3D::LookAtGameObject(GameObject * obj)
{
	ComponentTransform* trans_cmp = (ComponentTransform*)obj->FindComponent(COMP_TRANSFORMATION);
	if (trans_cmp == nullptr || obj->GetBoundingBoxDiagonalSize() <= 0)return;

	focus_point = trans_cmp->GetPosition();
	Look(focus_point, true);

	editor_camera_frustrum.pos = focus_point - (editor_camera_frustrum.front * obj->GetBoundingBoxDiagonalSize() * CAMERA_DIST_ON_FOCUS);
}

float* ModuleCamera3D::GetViewMatrix() const
{
	return editor_camera_frustrum.ViewMatrix().ptr();
}

float* ModuleCamera3D::GetViewMatrixTransposed() const
{
	return editor_camera_frustrum.WorldMatrix().ptr();
}

float* ModuleCamera3D::GetProjectionMatrix() const
{
	return editor_camera_frustrum.ViewProjMatrix().ptr();
}

float* ModuleCamera3D::GetProjectionMatrixTransposed() const
{
	return editor_camera_frustrum.ViewProjMatrix().Transposed().ptr();
}

float ModuleCamera3D::GetFrustrumNearPlaneDistance() const
{
	return editor_camera_frustrum.nearPlaneDistance;
}

float ModuleCamera3D::GetFrustrumFarPlaneDistance() const
{
	return editor_camera_frustrum.farPlaneDistance;
}

math::float3 ModuleCamera3D::GetPosition() const
{
	return editor_camera_frustrum.pos;
}
