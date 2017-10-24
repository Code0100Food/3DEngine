#include "Globals.h"

#include <math.h>

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
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	position = vec3(0.0f, 0.0f, 5.0f);
	reference = vec3(0.0f, 0.0f, 0.0f);
	
	camera_location = vec3(0.0f, 0.0f, 0.0f);
	view_vector = vec3(0.0f, 0.0f, 0.0f);
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
	camera_location.x = json_array_get_number(_array, 0);
	camera_location.y = json_array_get_number(_array, 1);
	camera_location.z = json_array_get_number(_array, 2);
	//Load view vector
	_array = json_object_get_array(data_root, "view_vector");
	view_vector.x = json_array_get_number(_array, 0);
	view_vector.y = json_array_get_number(_array, 1);
	view_vector.z = json_array_get_number(_array, 2);
	//Load camera distance
	camera_dist = json_object_get_number(data_root, "camera_distance");
	//Load camera velocities
	camera_z_mov_vel = json_object_get_number(data_root, "camera_z_mov_vel");
	rot_around_vel = json_object_get_number(data_root, "rot_around_vel");
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

	return ret;
}

update_status ModuleCamera3D::Update(float dt)
{
	//Camera only accept input if the mouse is over the viewport
	if (App->renderer3D->GetMouseOnWorkspace())
	{
		//Camera zoom in/out
		if (App->input->GetMouseZ() == 1)
		{
			camera_dist -= camera_z_mov_vel;
		}
		else if (App->input->GetMouseZ() == -1)
		{
			camera_dist += camera_z_mov_vel;
		}

		//Camera rotate around point
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
		{
			int dx = App->input->GetMouseXMotion();
			int dy = -App->input->GetMouseYMotion();

			camera_location -= view_vector;

			if (dx != 0)
			{
				float DeltaX = (float)dx * rot_around_vel;

				X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
				Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
				Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			}

			if (dy != 0)
			{
				float DeltaY = (float)dy * rot_around_vel;

				Y = rotate(Y, DeltaY, X);
				Z = rotate(Z, DeltaY, X);
			}

			if (Y.y > 0.0f)camera_location = view_vector + Z * length(camera_location);
		}

		//Camera stafe 
		if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
		{
			int dx = -App->input->GetMouseXMotion();
			int dy = -App->input->GetMouseYMotion();

			if (dx != 0)
			{
				float DeltaX = (float)dx * strafe_vel;

				camera_location -= X * DeltaX;
				view_vector -= X * DeltaX;
			}
			if (dy != 0)
			{
				float DeltaY = (float)dy * strafe_vel;

				camera_location -= Y * DeltaY;
				view_vector -= Y * DeltaY;
			}

		}

		//Camera wasd movement
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			{
				camera_location += Z * wasd_vel;
				view_vector += Z * wasd_vel;
			}
			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			{
				camera_location -= Z * wasd_vel;
				view_vector -= Z * wasd_vel;
			}
			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			{
				camera_location += X * wasd_vel;
				view_vector += X * wasd_vel;
			}
			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			{
				camera_location -= X * wasd_vel;
				view_vector -= X * wasd_vel;
			}
			if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT)
			{
				camera_location += Y * wasd_vel;
				view_vector += Y * wasd_vel;
			}
			if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT)
			{
				camera_location -= Y * wasd_vel;
				view_vector -= Y * wasd_vel;
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
		}
	}

	//Look the vehicle body with the CameraLocation & the ViewVector & the Z_DIST defined
	App->camera->Look(camera_location + camera_dist * normalize(camera_location - view_vector), view_vector, true);
		
	// Recalculate matrix -------------
	CalculateViewMatrix();

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
	ImGui::InputFloat3("Camera Location", &camera_location, 2);
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Change main camera location.");

	//View vector ui
	ImGui::InputFloat3("View Vector", &view_vector, 2);
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Change main camera view vector.");

	//Camera dist ui
	if (ImGui::DragFloat("Camera Distance", &camera_dist, 0.1, NULL, NULL, "%.1f"))
	{
		App->audio->PlayFxForInput(FX_ID::SLICE_TICK_FX);
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Change main camera distance from the view target.");

	ImGui::Separator();
	
	//Camera Velocities ui
	ImGui::Text("Sensibilities");
	ImGui::SliderFloat("Zoom Sensibility", &camera_z_mov_vel, 0.1f, 10.0f, "%.1f");
	ImGui::SliderFloat("Rotate around Sensibility", &rot_around_vel, 0.1f, 10.0f, "%.1f");
	ImGui::SliderFloat("Stafe Sensibility", &strafe_vel, 0.1f, 10.0f, "%.1f");
	ImGui::SliderFloat("WASD Sensibility", &wasd_vel, 0.1f, 10.0f, "%.1f");
}

void ModuleCamera3D::SaveConfigInfo(JSON_Object * data_root)
{
	//Save camera location
	json_array_t* _array = json_object_get_array(data_root, "camera_location");
	json_array_replace_number(_array, 0, camera_location.x);
	json_array_replace_number(_array, 1, camera_location.y);
	json_array_replace_number(_array, 2, camera_location.z);
	//Save view vector
	_array = json_object_get_array(data_root, "view_vector");
	json_array_replace_number(_array, 0, view_vector.x);
	json_array_replace_number(_array, 1, view_vector.y);
	json_array_replace_number(_array, 2, view_vector.z);
	//Save camera distance
	json_object_set_number(data_root, "camera_distance", camera_dist);
	//Save camera velocities
	json_object_set_number(data_root, "camera_z_mov_vel", camera_z_mov_vel);
	json_object_set_number(data_root, "rot_around_vel", rot_around_vel);
	json_object_set_number(data_root, "strafe_vel", strafe_vel);
	json_object_set_number(data_root, "wasd_vel", wasd_vel);
}

// Functionality ================================
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->position = Position;
	this->reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->reference = this->position;
		this->position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	reference = Spot;

	Z = normalize(position - reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}

void ModuleCamera3D::LookAtGameObject(GameObject * obj)
{
	ComponentTransform* trans_cmp = (ComponentTransform*)obj->FindComponent(COMP_TRANSFORMATION);
	if (trans_cmp == nullptr || obj->GetBoundingBoxDiagonalSize() <= 0)return;

	view_vector = trans_cmp->GetPosition();
	camera_dist = obj->GetBoundingBoxDiagonalSize() * CAMERA_DIST_ON_FOCUS;
	if(App->renderer3D->GetMaxRenderDistance() < camera_dist * RENDER_DIST_ON_FOCUS)App->renderer3D->SetMaxRenderDistance(camera_dist * RENDER_DIST_ON_FOCUS);
	camera_location = view_vector + camera_dist * Z;
}

void ModuleCamera3D::Move(const vec3 &Movement)
{
	position += Movement;
	reference += Movement;

	CalculateViewMatrix();
}

float* ModuleCamera3D::GetViewMatrix()
{
	return &view_matrix;
}

void ModuleCamera3D::CalculateViewMatrix()
{
	view_matrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, position), -dot(Y, position), -dot(Z, position), 1.0f);
	view_matrix_inverse = inverse(view_matrix);
}
