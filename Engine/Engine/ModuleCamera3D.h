#ifndef _MODULE_CAMERA_3D_H_
#define _MODULE_CAMERA_3D_H_

#include "Module.h"
#include "Globals.h"
#include "glmath.h"

struct PhysVehicle3D;

class ModuleCamera3D : public Module
{
public:

	ModuleCamera3D(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~ModuleCamera3D();

public:

	bool			Awake(const JSON_Object* data_root);
	bool			Start() final;
	update_status	Update(float dt) final;
	bool			CleanUp() final;
	void			BlitConfigInfo();
	void			SaveConfigInfo(JSON_Object* data_root);

public:

	vec3	X, Y, Z, position, reference;

private:

	vec3	camera_location;
	vec3	view_vector;
	float	camera_dist = 0;
	mat4x4	view_matrix, view_matrix_inverse;
	float	camera_z_mov_vel = 0.0f;
	float	rot_around_vel = 0.0f;
	float	strafe_vel = 0.0f;
	float	wasd_vel = 0.0f;

public:

	void	Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void	LookAt(const vec3 &Spot);
	void	Move(const vec3 &Movement);
	float*	GetViewMatrix();

private:

	void	CalculateViewMatrix();

};


#endif // !_MODULE_CAMERA_3D_H_

