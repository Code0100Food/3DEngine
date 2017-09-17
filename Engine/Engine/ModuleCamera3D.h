#ifndef _MODULE_CAMERA_3D_H_
#define _MODULE_CAMERA_3D_H_


#include "Module.h"
#include "Globals.h"
#include "glmath.h"

struct PhysVehicle3D;

class ModuleCamera3D : public Module
{
public:

	ModuleCamera3D(Application* app, bool start_enabled = true);

public:

	bool			Start() final;
	update_status	Update(float dt) final;
	bool			CleanUp() final;

public:

	bool	camera_debug = false;
	vec3	X, Y, Z, Position, Reference;

private:

	float			camera_speed = 0.0f;
	PhysVehicle3D*	Target = nullptr;
	vec3			CameraLocation;
	vec3			ViewVector;
	int				camera_dist = 0;
	vec3			VehicleLocation;
	uint			camera_fx = 0;
	mat4x4			ViewMatrix, ViewMatrixInverse;

public:

	void	Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void	LookAt(const vec3 &Spot);
	void	Move(const vec3 &Movement);
	float*	GetViewMatrix();

private:

	void	CalculateViewMatrix();

};


#endif // !_MODULE_CAMERA_3D_H_

