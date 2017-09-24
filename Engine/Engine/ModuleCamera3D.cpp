#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"

// Constructors =================================
ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
	name = "Camera";

	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	position = vec3(0.0f, 0.0f, 5.0f);
	reference = vec3(0.0f, 0.0f, 0.0f);
	
	camera_location = vec3(0.0f, 0.0f, 0.0f);
	view_vector = vec3(0.0f, 0.0f, 0.0f);
}

// Game Loop ====================================
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;
	
	config_menu = true;

	camera_location = vec3(5.0f, 2.0f, 5.0f);
	view_vector = vec3(0.0f,1.0f, 0.0f);
	camera_dist = 2;

	return ret;
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
	//View vector ui
	ImGui::InputFloat3("View Vector", &view_vector, 2);
	//Camera dist ui
	ImGui::InputFloat("Camera Distance", &camera_dist, 0.1, 0.5, 1);

}

update_status ModuleCamera3D::Update(float dt)
{
	//Look the vehicle body with the CameraLocation & the ViewVector & the Z_DIST defined
	App->camera->Look(camera_location + camera_dist* normalize(camera_location - view_vector), view_vector, true);
		
	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
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
