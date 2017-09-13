#ifndef _MODULE_PLAYER_
#define _MODULE_PLAYER_
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "ModulePhysics3D.h"
#include "Timer.h"

struct PhysVehicle3D;

#define MAX_ACCELERATION 10000.0f
#define TURN_DEGREES 10.0f * DEGTORAD
#define BRAKE_POWER 1000.0f

class ModulePlayer : public Module
{
public:
	
	ModulePlayer(Application* app, bool start_enabled = true);
	~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	mat4x4 chasis_loc;

	bool god = false;
	uint checkpoint_num = 0;

public:

	PhysVehicle3D* vehicle;
	
	PhysBody3D* door_1;
	PhysBody3D* door_2;
	PhysBody3D* Back_Door;
	
	Cube print_door_1;
	Cube print_door_2;
	Cube print_Back_Door;
	
	btHingeConstraint* door_1_constrain;
	btHingeConstraint* door_2_constrain;
	btHingeConstraint* Back_Door_constrain;
	
	float turn;
	float acceleration;
	float brake;

	uint hydraulic_suspension_fx;
	uint car_reset_fx;

	uint engine_low_rpm_fx;
	uint engine_mid_rpm_fx;
	uint engine_high_rpm_fx;
	uint engine_max_rpm_fx;
	uint doors_crash_fx;

	Timer	engine_timer;
	uint	engine_loop = 330;
	float	engine_rpm = 0;
	uint	engine_low_vol = 20;
	uint	engine_high_vol = 35;
	float	engine_current_vol = engine_low_vol;

	Timer	turbo_timer;
	uint	turbo_rate = 2500;
	uint	turbo_fx;

	Timer	lights_timer;
	bool	lights_on = false;
	bool	up_light_on = false;
	bool	back_light_on = false;
	uint	lights_rate = 20;
	Timer	break_timer;
	uint	break_rate = 65;

	Timer	chronometer;
	uint	seconds = 0;
	uint	decimes = 0;
	uint	minutes = 0;
	uint	record_sec = 9;
	uint	record_dec = 9;
	uint	record_min = 9;

	bool	InLap = false;

	bool alive;
	int current_time;
	Timer Respawn_time;

public:

	btRigidBody*		GetVehicleBody()const;
	void				ResetPlayer();
	void				ResetPlayerTime();
	void				RespawnPlayer();
	bool				CheckRecord();
	bool				Is_Vehicle_part(PhysBody3D* body);
	void				DestroyCar();
};

#endif;