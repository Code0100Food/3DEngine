#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"
#include "ModuleAudio.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
	chronometer.Stop();
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");
	hydraulic_suspension_fx = App->audio->LoadFx("Game/Audio/hydraulic_suspension_fx.wav");
	car_reset_fx = App->audio->LoadFx("Game/Audio/reset.wav");

	engine_low_rpm_fx = App->audio->LoadFx("Game/Audio/engine_low_rpm.wav");
	engine_mid_rpm_fx = App->audio->LoadFx("Game/Audio/engine_mid_rpm.wav");
	engine_high_rpm_fx = App->audio->LoadFx("Game/Audio/engine_high_rpm.wav");
	engine_max_rpm_fx = App->audio->LoadFx("Game/Audio/engine_max_rpm.wav");
	turbo_fx = App->audio->LoadFx("Game/Audio/turbo_fx.wav");
	doors_crash_fx = App->audio->LoadFx("Game/Audio/doors_crash_fx.wav");

	VehicleInfo car;
	//Car engine Build
	Mix_Volume(2, engine_low_vol);
	
	// Car properties ----------------------------------------
	car.chassis_size.Set(6, 5, 8);
	car.chassis_offset.Set(0, 3.5f, 0);
	car.mass = 1000.0f;
	car.suspensionStiffness = 50.0f;
	car.suspensionCompression = 0.0f;
	car.suspensionDamping = 8.88f;
	car.maxSuspensionTravelCm = 200.0f;
	car.frictionSlip = 50.5;
	car.maxSuspensionForce = 60000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.5f;
	float wheel_radius = 0.8f;
	float wheel_width = 0.8f;
	float suspensionRestLength = 1.2f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;

	vec3 direction(0, -1, 0);
	vec3 axis(-1, 0, 0);

	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width * 2;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width * 2;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	vehicle = App->physics->AddVehicle(car);

	print_door_1.ReSize(1, 2, 3);
	print_door_1.SetPos(vehicle->get_rigid_body()->getWorldTransform().getOrigin().x() + 2.5f, vehicle->get_rigid_body()->getWorldTransform().getOrigin().y() + 2.5f, vehicle->get_rigid_body()->getWorldTransform().getOrigin().z() + 1.0f);

	print_door_2.ReSize(1, 2, 3);
	print_door_2.SetPos(vehicle->get_rigid_body()->getWorldTransform().getOrigin().x() - 2.5f, vehicle->get_rigid_body()->getWorldTransform().getOrigin().y() + 2.5f, vehicle->get_rigid_body()->getWorldTransform().getOrigin().z() + 1.0f);

	print_Back_Door.ReSize(4, 4, 1);
	print_Back_Door.SetPos(vehicle->get_rigid_body()->getWorldTransform().getOrigin().x(), vehicle->get_rigid_body()->getWorldTransform().getOrigin().y() + 4.0f, vehicle->get_rigid_body()->getWorldTransform().getOrigin().z() - 3.5f);


	//Setting up the car elements
	door_1 = App->physics->AddBody(&print_door_1, OBJECT_TYPE::DINAMIC_CUBE);
	door_2 = App->physics->AddBody(&print_door_2, OBJECT_TYPE::DINAMIC_CUBE);
	Back_Door = App->physics->AddBody(&print_Back_Door, OBJECT_TYPE::DINAMIC_CUBE);

	door_1->get_rigid_body()->setIgnoreCollisionCheck(vehicle->get_rigid_body(), true);
	door_1->get_rigid_body()->setIgnoreCollisionCheck(door_2->get_rigid_body(), true);
	door_1->get_rigid_body()->setIgnoreCollisionCheck(Back_Door->get_rigid_body(), true);

	door_2->get_rigid_body()->setIgnoreCollisionCheck(vehicle->get_rigid_body(), true);
	door_2->get_rigid_body()->setIgnoreCollisionCheck(door_2->get_rigid_body(), true);
	door_2->get_rigid_body()->setIgnoreCollisionCheck(Back_Door->get_rigid_body(), true);

	Back_Door->get_rigid_body()->setIgnoreCollisionCheck(vehicle->get_rigid_body(), true);
	Back_Door->get_rigid_body()->setIgnoreCollisionCheck(door_2->get_rigid_body(), true);
	Back_Door->get_rigid_body()->setIgnoreCollisionCheck(Back_Door->get_rigid_body(), true);

	//Setting hinges
	door_1_constrain = App->physics->Add_Hinge_Constraint(*vehicle->get_rigid_body(), *door_1->get_rigid_body(), { 2.5f, 3.0f, 2.5f }, { 0,0,-1.5f }, btVector3(0, 1, 0), btVector3(0, 1, 0));
	door_1_constrain->setLimit((3.1416f), (3 * (3.1416f / 2)));

	door_2_constrain = App->physics->Add_Hinge_Constraint(*vehicle->get_rigid_body(), *door_2->get_rigid_body(), { -2.5f, 3.0f, 2.5f }, { 0,0,-1.5f }, btVector3(0, 1, 0), btVector3(0, 1, 0));
	door_2_constrain->setLimit((3.1416f / 2), 3.1416f);

	Back_Door_constrain = App->physics->Add_Hinge_Constraint(*vehicle->get_rigid_body(), *Back_Door->get_rigid_body(), { 0, 6.0f, -3.5f }, { 0,2,0 }, btVector3(1, 0, 0), btVector3(1, 0, 0));
	Back_Door_constrain->setLimit((3 * (3.1416f / 2)), (3.1416f * 2));

	//Player Init Location
	RespawnPlayer();

	Respawn_time.Stop();

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

btRigidBody * ModulePlayer::GetVehicleBody() const
{
	if (vehicle != NULL)return vehicle->get_rigid_body();
	else return nullptr;
}

void ModulePlayer::ResetPlayer()
{
	checkpoint_num = 0;
	InLap = false;
	engine_current_vol = engine_low_vol;
	chronometer.Start();
	chronometer.Stop();
}

void ModulePlayer::ResetPlayerTime()
{
	seconds = 0;
	decimes = 0;
	minutes = 0;
}

void ModulePlayer::RespawnPlayer()
{
	uint spawn;
	if (checkpoint_num >= 0)spawn = checkpoint_num;
	else spawn = 0;
	mat4x4 transform;
	vehicle->SetTransform(&App->scene_intro->checkpoints[spawn]);
	vehicle->get_rigid_body()->getWorldTransform().getOpenGLMatrix(&transform);

	//Base
	vehicle->get_rigid_body()->setLinearVelocity({ 0,0,0 });
	vehicle->get_rigid_body()->setAngularVelocity({ 0,0,0 });
	//Left
	door_1->SetTransform(&transform);
	door_1->get_rigid_body()->setAngularVelocity({ 0,0,0 });
	door_1->get_rigid_body()->setLinearVelocity({ 0,0,0 });
	//Right
	door_2->SetTransform(&transform);
	door_2->get_rigid_body()->setAngularVelocity({ 0,0,0 });
	door_2->get_rigid_body()->setLinearVelocity({ 0,0,0 });
	//Back
	Back_Door->SetTransform(&transform);
	Back_Door->get_rigid_body()->setAngularVelocity({ 0,0,0 });
	Back_Door->get_rigid_body()->setLinearVelocity({ 0,0,0 });

	if (door_1_constrain->isEnabled() == false)
		door_1_constrain->setEnabled(true);

	if (door_2_constrain->isEnabled() == false)
		door_2_constrain->setEnabled(true);

	if (Back_Door_constrain->isEnabled() == false)
		Back_Door_constrain->setEnabled(true);

	alive = true;
}

bool ModulePlayer::CheckRecord()
{
	bool ret = false;
	if ((minutes * 60 + decimes * 10 + seconds) < (record_min * 60 + record_dec * 10 + record_sec))
	{
		record_min = minutes;
		record_dec = decimes;
		record_sec = seconds;
		ret = true;
	}
	minutes = 0;
	decimes = 0;
	seconds = 0;
	return ret;
}

bool ModulePlayer::Is_Vehicle_part(PhysBody3D * body)
{
	return (body == door_1 || body == door_2 || body == Back_Door || body == vehicle);
}

void ModulePlayer::DestroyCar()
{
	App->audio->PlayFx(App->player->doors_crash_fx);
	door_1_constrain->setEnabled(false);
	door_2_constrain->setEnabled(false);
	Back_Door_constrain->setEnabled(false);
	alive = false;
	Respawn_time.Start();
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	//Player Chronometer ---------------------------------------
	if (chronometer.Read() > 1000)
	{
		seconds++;

		if (seconds > 9)
		{
			seconds = 0;
			++decimes;
		}

		if (decimes > 5)
		{
			decimes = 0;
			++minutes;
		}
		chronometer.Start();
	}
	//Vehicle Data ---------------------------------------------
	vehicle->get_rigid_body()->getWorldTransform().getOpenGLMatrix(&chasis_loc);
	turn = acceleration = brake = 0.0f;
	bool contact = false;
	for (uint k = 0; k < 4; k++)
	{
		if (vehicle->vehicle->m_wheelInfo[k].m_raycastInfo.m_isInContact)
		{
			contact = true;
			break;
		}
	}

	//Car engine --------------------
	if (engine_timer.Read() > engine_loop)
	{
		Mix_Volume(2, engine_current_vol);
		if(engine_rpm < 105)App->audio->PlayFx(engine_low_rpm_fx,2);
		else if (engine_rpm < 200)App->audio->PlayFx(engine_mid_rpm_fx,2);
		else if(engine_rpm < 320)App->audio->PlayFx(engine_high_rpm_fx, 2);
		else App->audio->PlayFx(engine_max_rpm_fx, 2);
		engine_timer.Start();
	}
	if (contact)engine_rpm = vehicle->GetKmh();
	// -------------------


	//Car Mechanics --------------------------------------------

	if (alive)
	{
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		{
			if (contact)
			{
				acceleration = MAX_ACCELERATION;
				if (engine_current_vol < engine_high_vol)engine_current_vol += 0.3f;
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			if (contact)
			{
				if (vehicle->GetKmh() < 2.5f)acceleration = -MAX_ACCELERATION;
				else brake = BRAKE_POWER;
				if (engine_current_vol > engine_low_vol)engine_current_vol -= 0.7f;
			}

			if (vehicle->vehicle_lights[0].color.r < 1.5f && break_timer.Read() > break_rate)
			{
				vehicle->vehicle_lights[0].FadeColor(0.35f, 0, 0);
				vehicle->vehicle_lights[1].FadeColor(0.35f, 0, 0);
				break_timer.Start();
			}
		}

		// Car Jump ------------------------------------------------
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			if (contact) {
				mat4x4 trans;
				this->vehicle->GetTransform(&trans);
				vec3 PushVector(trans[4], trans[5], trans[6]);
				PushVector *= 24000;
				vehicle->Push(PushVector.x, PushVector.y, PushVector.z);
				App->audio->PlayFx(hydraulic_suspension_fx);
			}
		}


		btVector3 chasis_ang_vel = vehicle->get_rigid_body()->getAngularVelocity();

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			if (contact)
			{
				if (turn < TURN_DEGREES)	turn += TURN_DEGREES;
			}
			else
			{
				vehicle->get_rigid_body()->setAngularVelocity({ chasis_ang_vel.getX(),chasis_ang_vel.getY() + 0.04f,chasis_ang_vel.getZ() });
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			if (contact)
			{
				if (turn > -TURN_DEGREES)	turn -= TURN_DEGREES;
			}
			else
			{
				vehicle->get_rigid_body()->setAngularVelocity({ chasis_ang_vel.getX(),chasis_ang_vel.getY() - 0.04f,chasis_ang_vel.getZ() });
			}
		}

	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP)
	{
		vehicle->vehicle_lights[0].color = Gray;
		vehicle->vehicle_lights[1].color = Gray;
	}



	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_UP && engine_rpm > 275 && contact && turbo_timer.Read() > turbo_rate)
	{
		App->audio->PlayFx(turbo_fx, 1);
		turbo_timer.Start();
	}

	//Lights ON ------------------------------------------------
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
	{
		if (lights_on)
		{
			lights_on = false;
			up_light_on = false;
			vehicle->vehicle_lights[4].color = White;
			vehicle->vehicle_lights[5].color = White;
		}
		else
		{
			lights_on = true;
			up_light_on = true;
		}
	}

	if (lights_on && lights_timer.Read() > lights_rate)
	{
		if (up_light_on)
		{
			vehicle->vehicle_lights[4].FadeColor(0, 0.05f, 0.2);
			vehicle->vehicle_lights[5].FadeColor(0, -0.05f, -0.2);
			if (vehicle->vehicle_lights[4].color.g > 1.5f)up_light_on = false;
		}
		else if(up_light_on == false)
		{
			vehicle->vehicle_lights[4].FadeColor(0, -0.05f, -0.2);
			vehicle->vehicle_lights[5].FadeColor(0, 0.05f, 0.2);
			if (vehicle->vehicle_lights[4].color.g < 0.5f)up_light_on = true;
		}
		lights_timer.Start();
	}

	

	//Vehicle Respawn ------------------------------------------
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN && App->camera->camera_debug == false)
	{
		App->audio->PlayFx(car_reset_fx);
		RespawnPlayer();
	}

	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
	{
		mat4x4 trans;
		vehicle->get_rigid_body()->getWorldTransform().getOpenGLMatrix(&trans);

		if (checkpoint_num < App->scene_intro->checkpoints.Count() - 1)
		{
			App->scene_intro->check_graph[checkpoint_num]->SetColor(Checkpoint_Color);
			checkpoint_num++;
			App->scene_intro->check_graph[checkpoint_num]->SetColor(Checkpoint_Color);
		}
		else checkpoint_num = 0;
		RespawnPlayer();
	}

	//Reset the vehicle position -------------------------------
	else if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)
	{
		App->audio->PlayFx(car_reset_fx);
		ResetPlayer();
		ResetPlayerTime();
		RespawnPlayer();
		App->scene_intro->ResetCheckpoints();
	}

	
		
	 if (Respawn_time.Read() > 1500)
	{
		App->audio->PlayFx(car_reset_fx);
		RespawnPlayer();
		Respawn_time.Start();
		Respawn_time.Stop();
	}
	
	



	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);

	char title[80];
	sprintf_s(title, "Current Lap: %i:%i%i || Record Lap: %i:%i%i || %.1f Km/h ", minutes,decimes,seconds,record_min,record_dec,record_sec, vehicle->GetKmh() * 0.1f);
	App->window->SetTitle(title);

	door_1->GetTransform(print_door_1.transform.M);
	door_2->GetTransform(print_door_2.transform.M);
	Back_Door->GetTransform(print_Back_Door.transform.M);

	vehicle->Render();
	print_door_1.Render();
	print_door_2.Render();
	print_Back_Door.Render();

	return UPDATE_CONTINUE;
}

