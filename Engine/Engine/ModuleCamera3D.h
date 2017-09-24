#ifndef _MODULE_CAMERA_3D_H_
#define _MODULE_CAMERA_3D_H_

#include "Module.h"
#include "Globals.h"
#include "glmath.h"

struct PhysVehicle3D;

class ModuleCamera3D : public Module
{
public:

	ModuleCamera3D(bool start_enabled = true);

public:

	bool			Awake(const JSON_Object* data_root);
	bool			Start() final;
	update_status	Update(float dt) final;
	bool			CleanUp() final;
	void			BlitConfigInfo();

public:

	vec3	X, Y, Z, position, reference;

private:

	vec3	camera_location;
	vec3	view_vector;
	float	camera_dist = 0;
	mat4x4	view_matrix, view_matrix_inverse;

public:

	void	Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void	LookAt(const vec3 &Spot);
	void	Move(const vec3 &Movement);
	float*	GetViewMatrix();

private:

	void	CalculateViewMatrix();

};


#endif // !_MODULE_CAMERA_3D_H_

