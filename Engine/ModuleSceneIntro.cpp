#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "ModuleAudio.h"
#include "PhysVehicle3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	//Regge Rock Hip Hop Music
	//int k = Mix_Volume(-1, 128);
	App->audio->PlayMusic("Game/Audio/OMFG_Hello.ogg");
	Checkpoint_fx = App->audio->LoadFx("Game/Audio/Checkpoint_fx.wav");
	Loop_Complete_fx = App->audio->LoadFx("Game/Audio/loop_complete_fx.wav");
	car_fall_fx = App->audio->LoadFx("Game/Audio/fall_fx.wav");
	game_over_fx = App->audio->LoadFx("Game/Audio/game_over_fx.wav");

	//Scene Checkpoints ========================================
	//Checkpoint 0 (init)
	checkpoints.PushBack({ -0.04f,0.0f,-1.0f,0.0f,		0.0f,1.0f,0.0f,0.0f,		1.0f,0.0f,-0.04f,0.0f,		-0.3f,132.0f,0.4f,1.0f });
	//Checkpoint 1 (ramp reception)
	checkpoints.PushBack({ 0.0f,0.0f,-1.0f,0.0f,		0.0f,1.0f,0.0f,0.0f,		1.0f,0.0f,0.0f,0.0f,		290.0f,109.0f,0.0f,1.0f });
	//Checkpoint 2 (half up)
	checkpoints.PushBack({ 0.95f,0.0f,0.32f,0.0f,		0.0f,1.0f,0.0f,0.0f,		-0.32f,0.0f,0.95f,0.0f,		595.5f,103.0f,290.5f,1.0f });
	//Checkpoint 3 (half down)
	checkpoints.PushBack({ -0.95f,0.0f,-0.3f,0.0f,		0.0f,1.0f,0.0f,0.0f,		0.3f,0.0f,-0.95f,0.0f,		792.0f,103.0f,406.5f,1.0f });
	//Checkpoint 4 (stairs end)
	checkpoints.PushBack({ -0.95f,0.0f,-0.3f,0.0f,		0.0f,1.0f,0.0f,0.0f,		0.31f,0.0f,-0.95f,0.0f,		976.3f,172.0f,-154.7f,1.0f });
	//Checkpoint 5 (giant curve end)
	checkpoints.PushBack({ 0.95f,0.0f,0.3f,0.0f,		0.0f,1.0f,0.0f,0.0f,		-0.3f,0.0f,0.95f,0.0f,		403.5f,170.0f,-348.5f,1.0f });
	//Checkpoint 6 (pre final ramp)
	checkpoints.PushBack({ 0.5f,0.0f,-0.8f,0.0f,		0.0f,1.0f,0.0f,0.0f,		0.8f,0.0f,0.5f,0.0f,		-560.2f,68.5f,-348.0f,1.0f });

	//Scene Objects ============================================
	//Scene Floor
	Cube* floor_cube = new Cube(2500.0f, 1.0f, 2500.0f);
	floor_cube->SetColor(Green);
	floor = AddMapObject(floor_cube, STATIC_CUBE, 1.0f);

	// Initial Ramp ============================================
	float alpha = 0;
	Cube* cube = new Cube(0, 0, 0);
	Cube* cube_2 = new Cube(0, 0, 0);

	//Start Floor
	Cube* start_floor = new Cube(30.0f, 0.2f, 30.0f);
	start_floor->SetPos(0, start_floor->size.y + 120, 0);
	start_floor->SetColor(White);
	AddMapObject(start_floor, STATIC_CUBE, 1.0f, false, true);
	AddCentralColumns(start_floor, start_floor->size.x, 12.0f, 10.0f);


	// Checkpoint 0
	Cube* check = new Cube(start_floor->size.x * 2, start_floor->size.y, start_floor->size.z);
	AddAdjacentBody(start_floor, check, -90, Z, start_floor->size.y * 0.5f, 0, 0.0f, true);

	//Start Sec Wall
	Cube* ramp_wall = new Cube(0.3f, 8.0f, 30.0f);
	ramp_wall->SetPos(0 - start_floor->size.x * 0.5f + ramp_wall->size.x * 0.5f, start_floor->transform.translation().y + ramp_wall->size.y * 0.5f, 0);
	ramp_wall->SetColor(Gray);
	AddMapObject(ramp_wall, STATIC_CUBE);
	AddCentralColumns(ramp_wall, 4.0f, 45.0f, 4.0f);

	//Start Ramp
	alpha = 15;
	cube->ReSize(10.0f, .02f, 30.0f);
	cube->SetColor(Gray);
	//cube_2 = cube;
	cube_2->ReSize(10.0f, .02f, 30.0f);
	cube_2->SetColor(Gray);

	AddAdjacentBody(start_floor, cube, alpha, Z);
	AddCentralColumns(cube, 2.0f, 4.0f, 2.0f);

	for (uint k = 0; k < 6; k++) {
		cube_2 = new Cube(cube);
		AddAdjacentBody(cube, cube_2, alpha * 0.15f, Z);
		AddCentralColumns(cube, 2.0f, 4.0f, 2.0f);
		cube = cube_2;
	}

	for (uint k = 0; k < 10; k++) {
		cube_2 = new Cube(cube);
		AddAdjacentBody(cube, cube_2, -alpha * 0.3f, Z);
		AddCentralColumns(cube, 2.0f, 4.0f, 2.0f);
		cube = cube_2;
	}
	alpha = 10;
	for (uint k = 0; k < 10; k++) {
		cube_2 = new Cube(cube);
		AddAdjacentBody(cube, cube_2, alpha * 0.15f, Z);
		if (k < 4)AddCentralColumns(cube, 2.0f, 4.0f, 2.0f);
		cube = cube_2;
	}
	// =========================================================


	// High Reception ==========================================
	Cube* high_reception = new Cube(80.0f, 0.2f, 30.0f);
	high_reception->SetPosFrom(cube, 0 + cube->size.x * 0.5f + high_reception->size.x * 0.5f, 0, 0);
	AddExternalColumns(high_reception, 5.0f, 5.0f, 5.0f);
	AddMapObject(high_reception, STATIC_CUBE, 1.0f, false, true);
	// =========================================================

	//Checkpoint 1 =============================================
	check->ReSize(50.0f, 0.5f, 30.0f);
	check->SetRotation(90, { 0,0,1 });
	check->SetPosFrom(high_reception, 0, 25.0f, 0);

	AddMapObject(check, SENSOR_CUBE, 1.0f, true);
	// =========================================================


	// Sky Curve ===============================================
	cube_2 = new Cube(cube_2);
	cube = new Cube(cube);
	cube->SetRotation(0, { 0,0,1 });
	cube->rotations = { 0,0,0 };
	cube->ReSize(40.0f, 0.2f, 30.f);
	cube_2->SetRotation(0, { 0,0,1 });
	cube_2->rotations = { 0,0,0 };
	cube_2->ReSize(40.0f, 0.2f, 30.f);

	alpha = 90.0f;

	cube->SetPosFrom(high_reception, 0 + high_reception->size.x * 0.5f + cube->size.x * 0.5f + 20.0f, 10.0f, 0);
	AddMapObject(cube, STATIC_CUBE);

	for (uint k = 0; k < 10; k++) {
		cube_2 = new Cube(cube);
		AddAdjacentBody(cube, cube_2, alpha * 0.12f, Y);
		AddCentralColumns(cube, 5.0f, 4.0f, 5.0f);
		cube = cube_2;
	}
	// =========================================================


	// Low Reception ===========================================
	Cube* low_reception = new Cube(45.0f, 0.2f, 60.f);
	low_reception->SetMultiRotation(cube->rotations.x, cube->rotations.y, cube->rotations.z);
	AddAdjacentBody(cube, low_reception, 0, Y, 0, -15, -30, false, true);
	AddExternalColumns(low_reception, 5.0f, 5.0f, 5.0f);
	// =========================================================

	//Checkpoint 2 =============================================
	check->ReSize(80.0f, 1.0f, 40.f);
	check->SetPosFrom(low_reception, 0, 20.0f, 0);
	check->SetRotation(90, { 1,0,0 });
	AddMapObject(check, SENSOR_CUBE, 1.0f, true);
	// =========================================================

	// Up Half =================================================
	alpha = -35.0f;
	cube = new Cube(cube);
	cube->ReSize(15.0f, 0.2f, 60.0f);
	AddAdjacentBody(low_reception, cube, 0, Y, 0, 0, 0);


	for (int k = 0; k < 20; k++) {
		cube_2 = new Cube(cube);
		if (k < 15)
		{
			cube_2->ReSize(cube_2->size.x, cube_2->size.y, cube_2->size.z + k*0.5f);
			AddAdjacentBody(cube, cube_2, alpha * 0.1, Z, 0.0f, 0.0f, -k);
		}
		else
		{
			AddAdjacentBody(cube, cube_2, alpha * 0.1, Z, 0.0f, 0.0f, 0.0f);
		}
		if (k % 2 == 0 && k < 15)AddCentralColumns(cube_2, 5.0f, 4.0f, 5.0f);
		cube = cube_2;
	}
	// =========================================================


	// Down Half Base ==========================================
	cube = new Cube(cube);
	cube->ReSize(35.0f, 0.2f, 60.0f);
	cube->rotations = low_reception->rotations;
	cube->SetColor(White);
	AddAdjacentBody(low_reception, cube, 0, Y, -20 * cos(alpha * 0.1 * DEGTORAD), 0, -cube->size.z - cube_2->size.z - low_reception->size.z, false, true);
	// =========================================================

	//Checkpoint 3 =============================================
	check = new Cube(cube);
	AddAdjacentBody(cube, check, -90, Z, 0.0f, 20.0f, 0.0f, true);
	// =========================================================


	// Down Half ===============================================
	cube = new Cube(cube);
	cube->SetColor(Gray);
	Cube* stairs_base = new Cube(cube);

	for (int k = 0; k < 20; k++) {
		cube_2 = new Cube(cube);
		cube_2->ReSize(15.0f, 0.2f, 60.0f + k * 4.5f);
		AddAdjacentBody(cube, cube_2, alpha * 0.1, Z, 0.0f, 0.0f, 0);
		if (k % 2 == 0 && k < 13)AddCentralColumns(cube_2, 5.0f, 4.0f, 5.0f);
		cube = cube_2;
	}
	// =========================================================


	// Super Base ==============================================
	stairs_base->rotations.y += 180;
	cube = new Cube(cube);
	cube->rotations = stairs_base->rotations;
	cube->ReSize(stairs_base->size.x + 65.0f, stairs_base->size.y, stairs_base->size.z);
	AddAdjacentBody(stairs_base, cube, 0, Y, 0, 0, 0);
	AddCentralColumns(cube, 5.0f, 33.0f, 5.0f);
	AddCentralColumns(stairs_base, 5.0f, 25.0f, 5.0f);
	// =========================================================


	// Super Stairs ============================================
	cube_2 = new Cube(cube);
	cube_2->ReSize(13.0f, cube_2->size.y, cube_2->size.z);

	for (int k = 0; k < 4; k++) {

		AddAdjacentBody(cube, cube_2, 90, Z, 0, 0, 0);
		cube_2 = new Cube(cube_2);
		cube_2->ReSize(cube_2->size.x, cube_2->size.y, cube_2->size.z - 7.5f);
		cube = new Cube(cube);
		cube->ReSize(cube->size.x, cube->size.y, cube->size.z - 7.5f);
		AddAdjacentBody(cube_2, cube, -90, Z, 0, 0, 0);
		AddCentralColumns(cube_2, 5.0f, 35.0f, 5.0f);
	}
	// =========================================================


	// Post Stairs Curve Base ==================================
	Cube* stairs_curve_start = new Cube(cube);
	stairs_curve_start->ReSize(35.0f, cube->size.y, cube->size.z);
	stairs_curve_start->SetPosFrom(cube, 0.0f, 5.0f, 0.0f);
	stairs_curve_start->SetColor(White);
	AddAdjacentBody(cube, stairs_curve_start, 0, Z, -50, 15, 0, false, true);
	AddExternalColumns(stairs_curve_start, 5.0f, 15.0f, 5.0f);
	// =========================================================


	//Checkpoint 4 =============================================
	cube = new Cube(cube);
	AddAdjacentBody(stairs_curve_start, cube, 90, Z, 0.0f, 20.0f, 0.0f, true);
	// =========================================================


	//Wind Mills ===============================================
	Cube* fix = new Cube(5.0f, 5.0f, 5.0f);
	fix->SetColor(Black);
	PhysBody3D* base;
	Cube* stick = new Cube(1.5f, 95.0f, 10.5f);
	stick->SetColor(Column_Color);
	PhysBody3D* helix;

	// Post Stairs Curve =======================================
	alpha = -180.0f;
	cube = new Cube(cube);
	cube->ReSize(20.0f, 0.02f, 30.0f);

	AddAdjacentBody(stairs_curve_start, cube, 0, Y, 0, 0, 0);

	for (int k = 0; k < 50; k++) {
		cube_2 = new Cube(cube);
		AddAdjacentBody(cube, cube_2, alpha * 0.02f, Y);
		if (k % 4 == 0) AddCentralColumns(cube, 5.0f, 4.0f, 5.0f);
		if (k % 17 == 0)
		{
			fix->rotations = cube->rotations;

			if (cube->rotations.y > 180) base = AddAdjacentBody(cube, fix, 0, Y, cube->size.x * 0.5f, 50, -cube->size.z * 0.5f);
			else base = AddAdjacentBody(cube, fix, 180, Y, cube->size.x * 0.5f, 50, -cube->size.z * 0.5f);

			helix = AddSceneObject(stick, OBJECT_TYPE::DINAMIC_CUBE);

			helix->get_rigid_body()->setWorldTransform(base->get_rigid_body()->getWorldTransform());
			helix->SetPos(base->get_rigid_body()->getWorldTransform().getOrigin().x(), base->get_rigid_body()->getWorldTransform().getOrigin().y(), base->get_rigid_body()->getWorldTransform().getOrigin().z());
			App->physics->Add_EnginedHinge_Constraint(*helix->get_rigid_body(), *base->get_rigid_body(), btVector3(0, 0, 0), btVector3(0, 0, 0), btVector3(0, 0, 1), btVector3(1, 0, 0));

			Wind_mills.PushBack(helix);
			if (k < 35)
			{
				fix = new Cube(fix);
				stick = new Cube(stick);
			}
		}
		cube = cube_2;
	}
	// =========================================================


	// Two Ways Ramp & Checkpoint 5 ============================
	alpha = 20;

	Cube* two_ways_ramp = new Cube(cube);
	two_ways_ramp->SetColor(White);
	AddAdjacentBody(cube, two_ways_ramp, alpha * 0.1f, Z, 0, 0, 0, false, true);
	cube_2 = new Cube(cube);
	AddAdjacentBody(two_ways_ramp, cube_2, -90, Z, 0.0f, 20.0f, 0.0f, true);

	for (int k = 0; k < 9; k++) {

		cube_2 = new Cube(cube_2);
		cube_2->ReSize(cube_2->size.x, cube_2->size.y, cube_2->size.z + k * 2);
		AddAdjacentBody(cube, cube_2, alpha * 0.1, Z, 0.0f, 0.0f, -k);
		if (k % 2 == 0)AddCentralColumns(cube, 5.0f, 4.0f, 5.0f);
		cube = cube_2;

	}
	Cube* ramp_base = cube;
	// =========================================================


	// Left Ramp Base ==========================================
	Cube* left_ramp_base = new Cube(cube);
	left_ramp_base->ReSize(cube->size.x, cube->size.y, cube->size.z * 0.5f);
	ramp_base->AddAdjacentBody(left_ramp_base, -15, Z, 40, -25, cube->size.z * 0.7);
	AddMapObject(left_ramp_base, STATIC_CUBE);
	// =========================================================

	// Left Ramp ===============================================
	alpha = 50;
	cube = new Cube(left_ramp_base);
	for (int k = 0; k < 5; k++) {
		cube_2 = new Cube(cube);
		AddAdjacentBody(cube, cube_2, alpha * 0.1, Z, 0, 0, +k * 0.25f);
		cube_2->ReSize(cube->size.x, cube->size.y, cube->size.z - k * 0.5f);
		AddCentralColumns(cube, cube->size.x * 0.5f, 15.0f, 0.4f);
		cube = cube_2;

	}
	for (int k = 0; k < 5; k++) {
		cube_2 = new Cube(cube);
		AddAdjacentBody(cube, cube_2, -alpha * 0.1, Z, 0, 0, 0);
		AddCentralColumns(cube, cube->size.x * 0.5f, 15.0f, 0.4f);
		cube = cube_2;

	}
	cube_2 = new Cube(cube);
	AddAdjacentBody(cube, cube_2, -cube->rotations.z, Z, 0, 0, 0);
	AddCentralColumns(cube, cube->size.x * 0.5f, 15.0f, 0.4f);
	// =========================================================


	// Left Zig Zag ============================================
	alpha = -60;
	cube = new Cube(cube_2);
	cube->ReSize(cube->size.x * 0.5, cube->size.y, cube->size.z);

	for (int k = 0; k < 12; k++) {
		cube_2 = new Cube(cube);
		AddAdjacentBody(cube, cube_2, alpha * 0.0833, Y, 0, 0, 0);
		if (k % 2 == 0 || k == 1)AddCentralColumns(cube, 2.0f, 15.0f, 2.0f);
		cube = cube_2;

	}

	alpha = 5 + alpha*-1;
	for (int k = 0; k < 12; k++) {
		cube_2 = new Cube(cube);
		AddAdjacentBody(cube, cube_2, alpha * 0.0833, Y, 0, 0, 0);
		if (k % 2 == 0)AddCentralColumns(cube, 2.0f, 15.0f, 2.0f);
		cube = cube_2;

	}

	alpha = -15;
	for (int k = 0; k < 10; k++) {
		cube_2 = new Cube(cube);
		AddAdjacentBody(cube, cube_2, alpha * 0.1, Z, 0, 0, 0);
		if (k % 2 == 0)AddCentralColumns(cube, 2.0f, 15.0f, 2.0f);
		cube = cube_2;

	}
	AddCentralColumns(cube_2, 2.0f, 15.0f, 2.0f);

	Cube* post_ramp_base = cube;
	// =========================================================


	// Right Ramp Base =========================================
	Cube* right_ramp_base = new Cube(left_ramp_base);
	AddAdjacentBody(ramp_base, right_ramp_base, -30, Z, 40, -10, -cube->size.z * 0.1);
	// =========================================================


	// Right Ramp ==============================================
	alpha = 10;
	cube = new Cube(right_ramp_base);

	for (int k = 0; k < 17; k++) {
		cube_2 = new Cube(cube);
		AddAdjacentBody(cube, cube_2, alpha * 0.1, Z, 0, 0, 0);
		if (k < 6 || (k > 8 && k < 13))AddCentralColumns(cube, cube->size.x * 0.5f, 15.0f, 0.4f);
		cube = cube_2;

	}
	// =========================================================


	// Post Ramps Base =========================================
	cube = new Cube(cube);
	cube->ReSize(cube->size.x * 1.5, cube->size.y, cube->size.z * 1.5f);
	AddAdjacentBody(post_ramp_base, cube, 15, Z, 75, 0, -cube->size.z * 0.2f);
	cube_2 = cube;
	// =========================================================


	// Pre Curve ===============================================
	alpha = 35;
	for (int k = 0; k < 10; k++) {
		cube_2 = new Cube(cube);
		AddAdjacentBody(cube, cube_2, alpha * 0.1, Y, 0, 0, 0);
		if (k % 3)AddCentralColumns(cube, 2.0f, 2.0f, 2.0f);
		cube = cube_2;

	}
	// =========================================================


	//Super Slalon =============================================
	alpha = 110;
	for (int k = 0; k < 21; k++) {
		cube_2 = new Cube(cube);
		AddAdjacentBody(cube, cube_2, alpha * 0.0666, Y, 0, 0, 0);
		if (k % 3)AddCentralColumns(cube, 2.0f, 2.0f, 2.0f);
		cube = cube_2;

	}

	alpha = -125;
	cube_2 = new Cube(cube);
	cube_2->ReSize(cube_2->size.x * 0.8f, cube_2->size.y, cube_2->size.z);
	for (int k = 0; k < 15; k++) {

		AddAdjacentBody(cube, cube_2, alpha * 0.0666, Y, 0, 0, 0);
		if (k % 3)AddCentralColumns(cube, 2.0f, 2.0f, 2.0f);
		cube = cube_2;
		if (k < 14)cube_2 = new Cube(cube);
	}
	alpha = 125;
	for (int k = 0; k < 15; k++) {
		cube_2 = new Cube(cube);
		AddAdjacentBody(cube, cube_2, alpha * 0.0666, Y, 0, 0, 0);
		if (k % 3)AddCentralColumns(cube, 2.0f, 2.0f, 2.0f);
		cube = cube_2;

	}
	alpha = -125;
	for (int k = 0; k < 15; k++) {
		cube_2 = new Cube(cube);
		AddAdjacentBody(cube, cube_2, alpha * 0.0666, Y, 0, 0, 0);
		if (k % 3)AddCentralColumns(cube, 2.0f, 2.0f, 2.0f);
		cube = cube_2;

	}

	alpha = 216;
	cube_2 = new Cube(cube);
	cube_2->ReSize(cube_2->size.x * 1.4f, cube->size.y, cube->size.z);
	for (int k = 0; k < 15; k++) {
		cube_2 = new Cube(cube);
		AddAdjacentBody(cube, cube_2, alpha * 0.0666, Y, 0, 0, 0);
		if (k % 3)AddCentralColumns(cube, 2.0f, 2.0f, 2.0f);
		cube = cube_2;

	}

	// =========================================================
	cube_2 = new Cube(cube_2);
	if (cube_2->rotations.y > 360)cube_2->rotations.y -= 360;
	cube = new Cube(cube_2);

	//Super Ramp ==========================================
	alpha = -10;
	int mark = (cube_2->size.z - start_floor->size.z + 18) * 0.06666;
	cube_2->SetColor(White);
	AddAdjacentBody(cube, cube_2, alpha * 0.0666, Z, 0, 0, 0, false, true);
	check = new Cube(cube_2);
	AddAdjacentBody(cube_2, check, -90, Z, 0.0f, 20.0f, 0.0f, true);
	cube = cube_2;

	for (int k = 0; k < 14; k++) {

		cube_2 = new Cube(cube);
		cube_2->SetColor(Gray);
		cube_2->ReSize(cube->size.x, cube->size.y, cube->size.z - mark);
		AddAdjacentBody(cube, cube_2, alpha * 0.0666, Z, 0, 0, +mark * 0.5f);
		if (k % 3)AddCentralColumns(cube, 2.0f, 2.0f, 2.0f);
		cube = cube_2;

	}

	alpha = 15;
	cube_2 = new Cube(cube);
	cube_2->ReSize(cube_2->size.x * 1.2f, cube->size.y, cube->size.z);
	for (int k = 0; k < 9; k++) {

		AddAdjacentBody(cube, cube_2, alpha * 0.0555, Z, 0, 0, 0);
		if (k % 3)AddCentralColumns(cube, 2.0f, 2.0f, 2.0f);
		cube = cube_2;
		if (k < 10)cube_2 = new Cube(cube_2);
	}

	//Loop End Checkpoint
	check = new Cube(start_floor);
	check->ReSize(check->size.x * 2, check->size.y, check->size.z);
	AddAdjacentBody(start_floor, check, -90, Z, 0, start_floor->size.x, 0.0f, true);
	// =========================================================



	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	
	//Phys bodies are deleted in ModulePhysics3D
	phys_bodies.clear();
	
	//Delete all the graphic bodies
	uint k = graph_bodies.count();
	for (int h = k - 1; h >= 0; h--) {
		delete graph_bodies[h];
		graph_bodies[h] = NULL;
	}
	graph_bodies.clear();

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	//Render the plane
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	//Check for the number of items (phys & graph)
	uint scene_items_num = phys_bodies.count();
	
	//Update scene items
	for (uint k = 0; k < scene_items_num; k++) {
		phys_bodies[k]->GetTransform(&graph_bodies[k]->transform);
		graph_bodies[k]->Render();
	}



	//Check for the number of items (phys & graph)
	uint map_items_num = map_graphs.Count();

	//Update map items (just paint)
	for (uint k = 0; k < map_items_num; k++) {
		map_graphs[k]->Render();
	}

	if (App->player->minutes == 9)
	{
		App->player->ResetPlayer();
		App->player->ResetPlayerTime();
		ResetCheckpoints();
		App->player->RespawnPlayer();
		App->audio->PlayFx(game_over_fx);
	}
	
	return UPDATE_CONTINUE;
}

mat4x4 ModuleSceneIntro::GetCheckpoint(uint index) const
{
	return checkpoints[index];
}

void ModuleSceneIntro::ResetCheckpoints()
{
	uint num = check_graph.Count();
	for (int j = 0; j < num; j++) {
		check_graph[j]->SetColor(White);
	}
}

void ModuleSceneIntro::OnCollision(PhysBody3D * body1, PhysBody3D * body2)
{
	
	if (body1 == (PhysBody3D*)App->player->vehicle)
	{
		uint num = check_bodies.Count();

		for(int k = 0; k < num; k++){
		
			if (body2 == check_bodies[k])
			{
				if (k == 0 && App->player->checkpoint_num == 0 && App->player->InLap == false) {
					App->player->chronometer.Start();
					App->player->InLap = true;
					check_graph[k]->SetColor(Checkpoint_Color);
					App->audio->PlayFx(Checkpoint_fx);
				}
				else if (App->player->checkpoint_num < k && (k - App->player->checkpoint_num) == 1 && k < 7)
				{
					check_graph[k]->SetColor(Checkpoint_Color);
					App->player->checkpoint_num = k;
					App->audio->PlayFx(Checkpoint_fx);
				}
				else if (k == 7 && App->player->checkpoint_num == 6)
				{
					App->audio->PlayFx(Loop_Complete_fx);
					App->player->chronometer.Stop();
					App->player->CheckRecord();
					App->player->ResetPlayer();
					App->player->RespawnPlayer();
					ResetCheckpoints();
				}
				break;
			}
		}

		if(body2 == this->floor && App->player->alive)
		{
			App->player->DestroyCar();
		}
	}


	if (((App->player->Is_Vehicle_part(body1) && Is_Wind_Mill(body2)) || (Is_Wind_Mill(body1) && App->player->Is_Vehicle_part(body2))) && App->player->alive)
	{
		App->player->DestroyCar();
	}

}

void ModuleSceneIntro::AddCentralColumns(Primitive * target, float x, float y, float z)
{
	//Get the target position
	vec3 target_pos = ((Cube*)target)->transform.translation();

	//Calculate the vector from the position to the center of the sides
	vec3 Apoint(0.0f, 0.0f, -(((Cube*)target)->size.z * 0.5f) - z * 0.5f);
	Apoint = (rotate(Apoint, target->rotations.y, { 0,-1.0f,0 }));

	//Create the columns whit the data calculated
	Cube* col_r = new Cube(x, target_pos.y + y, z, target_pos.x + Apoint.x, ((target_pos.y + y) * 0.5f) + Apoint.y, target_pos.z + Apoint.z);
	Cube* col_l = new Cube(x, target_pos.y + y, z, target_pos.x - Apoint.x, ((target_pos.y + y) * 0.5f) + Apoint.y, target_pos.z - Apoint.z);
	//Rotate the new columns
	col_r->SetRotation(target->rotations.y, { 0,-1.0f,0 });
	col_l->SetRotation(target->rotations.y, { 0,-1.0f,0 });
	//Set Columns Color
	col_r->SetColor(Column_Color);
	col_l->SetColor(Column_Color);
	//Add it to the scene
	AddMapObject(col_r, STATIC_CUBE);
	AddMapObject(col_l, STATIC_CUBE);
	
}

void ModuleSceneIntro::AddExternalColumns(Primitive * target, float x, float y, float z)
{
	//Get the target position
	vec3 target_pos = ((Cube*)target)->transform.translation();

	vec3 Apoint_up((((Cube*)target)->size.x * 0.5f) + x*0.5f, 0.0f, -(((Cube*)target)->size.z * 0.5f) - z * 0.5f);
	vec3 Apoint_down((((Cube*)target)->size.x * 0.5f) + x*0.5f, 0.0f, +(((Cube*)target)->size.z * 0.5f) + z * 0.5f);

	Apoint_up = (rotate(Apoint_up, target->rotations.y, { 0,-1.0f,0 }));
	Apoint_down = (rotate(Apoint_down, target->rotations.y, { 0,-1.0f,0 }));

	Cube* col_r_up = new Cube(x, target_pos.y + y, z, target_pos.x + Apoint_up.x, ((target_pos.y + y) * 0.5f) + Apoint_up.y, target_pos.z + Apoint_up.z);
	Cube* col_r_down = new Cube(x, target_pos.y + y, z, target_pos.x + Apoint_down.x, ((target_pos.y + y) * 0.5f) + Apoint_down.y, target_pos.z + Apoint_down.z);
	Cube* col_l_up = new Cube(x, target_pos.y + y, z, target_pos.x - Apoint_up.x, ((target_pos.y + y) * 0.5f) + Apoint_up.y, target_pos.z - Apoint_up.z);
	Cube* col_l_down = new Cube(x, target_pos.y + y, z, target_pos.x - Apoint_down.x, ((target_pos.y + y) * 0.5f) + Apoint_down.y, target_pos.z - Apoint_down.z);

	//Rotate the new columns
	col_r_up->SetRotation(target->rotations.y, { 0,-1.0f,0 });
	col_r_down->SetRotation(target->rotations.y, { 0,-1.0f,0 });
	col_l_up->SetRotation(target->rotations.y, { 0,-1.0f,0 });
	col_l_down->SetRotation(target->rotations.y, { 0,-1.0f,0 });
	//Set the color to the columns
	col_r_up->SetColor(Column_Color);
	col_r_down->SetColor(Column_Color);
	col_l_up->SetColor(Column_Color);
	col_l_down->SetColor(Column_Color);
	//Add it to the scene
	AddMapObject(col_r_up, STATIC_CUBE);
	AddMapObject(col_r_down, STATIC_CUBE);
	AddMapObject(col_l_up, STATIC_CUBE);
	AddMapObject(col_l_down, STATIC_CUBE);
}

PhysBody3D* ModuleSceneIntro::AddSceneObject(Primitive* object, OBJECT_TYPE object_type, float mass)
{
	graph_bodies.add(object);
	PhysBody3D* phys = App->physics->AddBody(object, object_type, mass);
	phys->collision_listeners.add(this);
	phys_bodies.add(phys);

	return phys;
}

PhysBody3D * ModuleSceneIntro::AddMapObject(Primitive * object, OBJECT_TYPE object_type, float mass, bool is_sensor, bool sensor_check)
{
	if (is_sensor == false) {

		map_graphs.PushBack(object);

		if (sensor_check)check_graph.PushBack(object);

	}

	PhysBody3D* phys = App->physics->AddBody(object, object_type, mass);

	if (is_sensor) {
		phys->SetAsSensor(is_sensor);
		check_bodies.PushBack(phys);
	}

	phys->collision_listeners.add(this);
	map_bodies.PushBack(phys);

	return phys;
}

PhysBody3D* ModuleSceneIntro::AddAdjacentBody(Primitive * origin, Primitive * target, float angle, AXIS axis, float x, float y, float z, bool is_sensor, bool sensor_check)
{
	//Calculate the point of the parent object
	vec3 Apoint;

	/*
	AA		AB
	0.......0
	.		.
	.		.
	.		.
	0.......0
	BA		BB

	*/

	//Apoint = AB
	if (angle < -1 && axis == Y && origin->rotations.y < 180) {
		Apoint.x = ((Cube*)origin)->size.x * 0.5f;
		Apoint.y = 0.0f;
		Apoint.z = ((Cube*)origin)->size.z * 0.5f;
	}
	//Apoint = AA
	else if (origin->rotations.y < 180) {
		Apoint.x = (((Cube*)origin)->size.x * 0.5f);
		Apoint.y = 0.0f;
		Apoint.z = -(((Cube*)origin)->size.z * 0.5f);
	}
	//Apoint = BA
	else if (angle < -1 && axis == Y && origin->rotations.y >= 180) {
		Apoint.x = -((Cube*)origin)->size.x * 0.5f;
		Apoint.y = 0.0f;
		Apoint.z = -((Cube*)origin)->size.z * 0.5f;
	}
	//Apoint = BB
	else {
		Apoint.x = -(((Cube*)origin)->size.x * 0.5f);
		Apoint.y = 0.0f;
		Apoint.z = (((Cube*)origin)->size.z * 0.5f);
	}

	if (origin->rotations.z < 180)Apoint = (rotate(Apoint, origin->rotations.z, { 0,0,-1.0f }));
	else Apoint = (rotate(Apoint, origin->rotations.z - 180, { 0,0,-1.0f }));

	if (origin->rotations.y < 180)Apoint = (rotate(Apoint, origin->rotations.y, { 0,-1.0f,0 }));
	else Apoint = (rotate(Apoint, origin->rotations.y - 180, { 0,-1.0f,0 }));

	Apoint = (rotate(Apoint, origin->rotations.x, { -1.0f,0,0 }));

	//Calculate the point in the child object(rotate parent y)
	vec3 Bpoint;
	//Bpoint from AB
	if (angle < -1 && axis == Y && origin->rotations.y < 180) {
		Bpoint.x = Apoint.x + (((Cube*)target)->size.x * 0.5f) + x;
		Bpoint.y = y;
		Bpoint.z = Apoint.z - (((Cube*)target)->size.z * 0.5f) + z;
	}
	//Bpoint from AA
	else if (origin->rotations.y < 180) {
		Bpoint.x = Apoint.x + (((Cube*)target)->size.x * 0.5f) + x;
		Bpoint.y = y;
		Bpoint.z = Apoint.z + (((Cube*)target)->size.z * 0.5f) + z;
	}
	//Bpoint from BA
	else if (angle < -1 && axis == Y && origin->rotations.y > 180) {
		Bpoint.x = Apoint.x - (((Cube*)target)->size.x * 0.5f) + x;
		Bpoint.y = y;
		Bpoint.z = Apoint.z + (((Cube*)target)->size.z * 0.5f) + z;
	}
	//Bpoint from BB
	else {
		Bpoint.x = Apoint.x - (((Cube*)target)->size.x * 0.5f) + x;
		Bpoint.y = y;
		Bpoint.z = Apoint.z - (((Cube*)target)->size.z * 0.5f) + z;
	}



	if (origin->rotations.z != 0) {

		Bpoint.y = Apoint.y + y;

	}

	//Vector form A to B
	vec3 vector(Bpoint.x - Apoint.x, Bpoint.y - Apoint.y, Bpoint.z - Apoint.z);

	//Rotate the entered angle
	switch (axis) {

	case AXIS::Y:

		if (origin->rotations.z < 180)vector = rotate(vector, origin->rotations.z, { 0,0,-1.0f });
		else vector = rotate(vector, origin->rotations.z - 180, { 0,0,-1.0f });
		if (origin->rotations.y < 180)vector = rotate(vector, origin->rotations.y + angle, { 0,-1.0f,0 });
		else vector = rotate(vector, origin->rotations.y - 180 + angle, { 0,-1.0f,0 });


		//Update the body angle
		target->rotations.x = origin->rotations.x;
		target->rotations.y = angle + origin->rotations.y;
		target->rotations.z = origin->rotations.z;

		target->SetMultiRotation(origin->rotations.x, angle + origin->rotations.y, origin->rotations.z);
		break;

	case AXIS::Z:

		if (origin->rotations.z < 180)vector = rotate(vector, origin->rotations.z + angle, { 0,0,-1.0f });
		else vector = rotate(vector, origin->rotations.z - 180 + angle, { 0,0,-1.0f });
		if (origin->rotations.y < 180)vector = rotate(vector, origin->rotations.y, { 0,-1.0f,0 });
		else vector = rotate(vector, origin->rotations.y - 180, { 0,-1.0f,0 });


		//Update the body angle
		target->rotations.x = origin->rotations.x;
		target->rotations.y = origin->rotations.y;
		target->rotations.z = angle + origin->rotations.z;

		target->SetMultiRotation(origin->rotations.x, origin->rotations.y, angle + origin->rotations.z);
		break;

	}
	//Set the data calculated
	target->SetPosFrom(origin, Apoint.x + vector.x, Apoint.y + vector.y, Apoint.z + vector.z);

	PhysBody3D* taget_body = AddMapObject(target, STATIC_CUBE,1.0f,is_sensor, sensor_check);

	return  taget_body;
}

bool ModuleSceneIntro::Is_Wind_Mill(PhysBody3D * body)
{
	int wind_count = Wind_mills.Count();

	for (int i = 0; i < wind_count; i++)
	{
		if (body == Wind_mills[i])
			return true;
	}
	return false;
}



