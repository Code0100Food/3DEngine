#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2List.h"
#include "Primitive.h"

#include "Engine/Bullet/include/btBulletDynamicsCommon.h"

// Recommended scale is 1.0f == 1 meter, no less than 0.2 objects
#define GRAVITY btVector3(0.0f, -20.0f, 0.0f) 

class DebugDrawer;
struct PhysBody3D;
struct PhysVehicle3D;
struct VehicleInfo;

enum BODY_MASK
{
	NOTHING = 1,
	CAR = 2,
	MAP = 4,
	SENSOR = 8
};

class ModulePhysics3D : public Module
{
public:
	ModulePhysics3D(Application* app, bool start_enabled = true);
	~ModulePhysics3D();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	//Methods to add bodies to the world
	PhysBody3D* AddBody(const Primitive* primitive, enum OBJECT_TYPE object_type, float mass = 1.0f, BODY_MASK I_am = NOTHING);
	PhysVehicle3D* AddVehicle(const VehicleInfo& info);

	//Methods for constrains
	btPoint2PointConstraint*	Add_P2P_Constraint(btRigidBody& rbA, btRigidBody& rbB, const btVector3& pivotInA, const btVector3& pivotInB);
	btHingeConstraint*			Add_Hinge_Constraint(btRigidBody & rbA, btRigidBody & rbB, const btVector3& pivotInA, const btVector3& pivotInB, btVector3& axisInA, btVector3& axisInB);
	btHingeConstraint*			Add_EnginedHinge_Constraint(btRigidBody & rbA, btRigidBody & rbB, const btVector3& pivotInA, const btVector3& pivotInB, btVector3& axisInA, btVector3& axisInB);
	btFixedConstraint*			Add_Fixed_Constraint(btRigidBody& rbA, btRigidBody& rbB, const btTransform& frameA, const btTransform& frameB);

private:

	bool debug;

	btDefaultCollisionConfiguration*		collision_conf;
	btCollisionDispatcher*					dispatcher;
	btBroadphaseInterface*					broad_phase;
	btSequentialImpulseConstraintSolver*	solver;
	btDiscreteDynamicsWorld*				world;
	btDefaultVehicleRaycaster*				vehicle_raycaster;
	DebugDrawer*							debug_draw;

	p2List<btCollisionShape*>			shapes;
	p2List<PhysBody3D*>					bodies;
	p2List<btTypedConstraint*>			constrains;
	p2List<PhysVehicle3D*>				vehicles;
	p2List<btDefaultMotionState*>		motions;

};

class DebugDrawer : public btIDebugDraw
{
public:
	DebugDrawer() : line(0,0,0)
	{}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
	void reportErrorWarning(const char* warningString);
	void draw3dText(const btVector3& location, const char* textString);
	void setDebugMode(int debugMode);
	int	 getDebugMode() const;

	DebugDrawModes mode;
	Line line;
	Primitive point;
};