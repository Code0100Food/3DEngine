#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "PhysBody3D.h"

struct PhysBody3D;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:

	//Physic bodies list
	p2List<PhysBody3D*>			phys_bodies;
	//Graphic bodies list
	p2List<Primitive*>			graph_bodies;

	//Map bodies array
	p2DynArray<PhysBody3D*>		map_bodies;
	//Map grahics array
	p2DynArray<Primitive*>		map_graphs;
	p2DynArray<PhysBody3D*>		Wind_mills;
	PhysBody3D*					floor;

	//Scene Checkpoints
	p2DynArray<mat4x4>			checkpoints;
	p2DynArray<PhysBody3D*>		check_bodies;
	p2DynArray<Primitive*>		check_graph;
	uint						Checkpoint_fx;
	uint						Loop_Complete_fx;
	uint						car_fall_fx;
	uint						game_over_fx;

public:

	mat4x4			GetCheckpoint(uint index)const;
	void			ResetCheckpoints();

	void			OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	void			AddCentralColumns(Primitive* target, float x, float y, float z);
	void			AddExternalColumns(Primitive* target, float x, float y, float z);
	
	PhysBody3D*		AddSceneObject(Primitive* object, OBJECT_TYPE object_type, float mass = 1.0f);
	PhysBody3D*		AddMapObject(Primitive* object, OBJECT_TYPE object_type, float mass = 1.0f, bool is_sensor = false, bool sensor_check = false);
	PhysBody3D*		AddAdjacentBody(Primitive* origin, Primitive* target, float angle, AXIS axis, float x = 0, float y = 0, float z = 0, bool is_sensor = false, bool sensor_check = false);

	bool			Is_Wind_Mill(PhysBody3D* body);
};
