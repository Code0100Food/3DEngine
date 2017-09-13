#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "PhysVehicle3D.h"
#include "ModuleAudio.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);
	
	Target = nullptr;
	CameraLocation = vec3(0.0f, 0.0f, 0.0f);
	ViewVector = vec3(0.0f, 0.0f, 0.0f);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	camera_speed = 6.0f;

	Target = App->player->vehicle;
	CameraLocation = vec3(0.0f, 15.0f, 0.0f);
	ViewVector = vec3(0.0f,10.05f, 0.0f);
	camera_dist = 27;
	camera_fx = App->audio->LoadFx("Game/Audio/camera_fx.wav");
	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	//Change Camera Mode =======================================
	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) {
		camera_debug = !camera_debug;
		App->audio->PlayFx(camera_fx);
	}
	// Free Camera =============================================
	if (camera_debug) {
		vec3 newPos(0, 0, 0);
		float speed = camera_speed * 20 * dt;
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			speed = camera_speed * 20 * dt;

		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) newPos.y += speed;
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) newPos.y -= speed;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

		if (App->input->GetKey(SDL_SCANCODE_KP_8) == KEY_DOWN) camera_speed += 6.0f;
		if ((App->input->GetKey(SDL_SCANCODE_KP_2) == KEY_DOWN) && camera_speed > 10.0f) camera_speed -= 6.0f;

		Position += newPos;
		Reference += newPos;

		// Mouse motion ============================================
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			int dx = -App->input->GetMouseXMotion();
			int dy = -App->input->GetMouseYMotion();

			float Sensitivity = 0.25f;

			Position -= Reference;

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

			Position = Reference + Z * length(Position);
		}
	}
	// Follow the Car ==========================================
	else
	{
		// Change Camera Prespective ===============================
		if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN) {
			if (camera_dist == 27)camera_dist += 24;
			else
			{
				camera_dist -= 24;
			}
			App->audio->PlayFx(camera_fx);

		}

		//16 element array (12 rotation(row major padded on the right by 1), and 3 translation 
		mat4x4 vehicle_array;
		Target->GetTransform(&vehicle_array);
		
		
		//Data In array:
		//X Vector -- [0][1][2]
		//Y Vector -- [4][5][6]
		//Z Vector -- [8][9][10]
		
		// -> Z Axis is the parallel to the camera view
		
		//Body Location -- [12][13][14]
		
		//In the right cells ([3][7][11][15]) there are value 1
		
		//Vehicle Axis
		X = vec3(vehicle_array[0], vehicle_array[1], vehicle_array[2]);
		Y = vec3(vehicle_array[4], vehicle_array[5], vehicle_array[6]);
		Z = vec3(vehicle_array[8], vehicle_array[9], vehicle_array[10]);

		//Body Location
		VehicleLocation = vehicle_array.translation();

		//Look the vehicle body with the CameraLocation & the ViewVector & the Z_DIST defined
		App->camera->Look((CameraLocation + VehicleLocation) - Z * camera_dist, ViewVector + VehicleLocation, true);
	}
	
	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}
