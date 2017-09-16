#ifndef __PhysBody3D_H__
#define __PhysBody3D_H__

#include <list>
#include "glmath.h"

using namespace std;

class btRigidBody;
class Module;

enum OBJECT_TYPE
{
	DINAMIC_CUBE,
	STATIC_CUBE,
	SENSOR_CUBE,
	
	DINAMIC_CYLINDER,
	STATIC_CYLINDER,
	SENSOR_CYLINDER,
	
	DINAMIC_PLANE,
	STATIC_PLANE,
	SENSOR_PLANE,
	
	DINAMIC_SPHERE,
	STATIC_SPHERE,
	SENSOR_SPHERE

};

struct PhysBody3D
{
public:

	PhysBody3D(btRigidBody* body);
	~PhysBody3D();

public:

	list<Module*> collision_listeners;

private:

	btRigidBody*	body = nullptr;
	bool			is_sensor = false;

public:

	void			Push(float x, float y, float z);
	void			GetTransform(float* matrix) const;
	void			SetTransform(const float* matrix) const;
	void			SetPos(float x, float y, float z);
	vec3			GetPos()const;
	btRigidBody*	get_rigid_body() const;
	void			SetAsSensor(bool is_sensor);
	bool			IsSensor() const;

};

#endif // __PhysBody3D_H__