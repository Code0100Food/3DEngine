#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleAudio.h"
#include "FileSystem.h"
#include "ModuleInput.h"

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
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		int dx = App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.25f;

		camera_location -= view_vector;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			Y = rotate(Y, DeltaY, X);
			Z = rotate(Z, DeltaY, X);

			if (Y.y < 0.0f)
			{
				Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = cross(Z, X);
			}
		}

		camera_location = view_vector + Z * length(camera_location);
		LOG("Z:		%.2f	%.2f	%.2f", Z.x, Z.y, Z.z);
	}

	//Camera move 
	/*if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		float Sensitivity = 0.25f;
		
		int dx = App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float DeltaY = (float)dy * Sensitivity;
		float DeltaX = (float)dx * Sensitivity;

		vec3 camera_view_dir = view_vector - position;
		vec3 view_dir_normal(-camera_view_dir.y, camera_view_dir.x, camera_view_dir.z);
		float len = sqrt(view_dir_normal.x*view_dir_normal.x + view_dir_normal.y*view_dir_normal.y + view_dir_normal.z*view_dir_normal.z);
		vec3 normalized = view_dir_normal / len;

		camera_location += normalized*DeltaX;
	}*/

	//Look the vehicle body with the CameraLocation & the ViewVector & the Z_DIST defined
	App->camera->Look(camera_location + camera_dist* normalize(camera_location - view_vector), view_vector, true);
		
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
