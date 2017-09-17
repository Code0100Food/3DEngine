#include "Globals.h"
#include "Application.h"
#include "ModulePhysics3D.h"
#include "PhysBody3D.h"
#include "Primitive.h"
#include "SDL\include\SDL.h"
#include "ModuleInput.h"
#include "ModuleCamera3D.h"

#ifdef _DEBUG
#pragma comment (lib, "Engine/Bullet/libx86/BulletDynamics_debug.lib")
#pragma comment (lib, "Engine/Bullet/libx86/BulletCollision_debug.lib")
#pragma comment (lib, "Engine/Bullet/libx86/LinearMath_debug.lib")
#else
#pragma comment (lib, "Engine/Bullet/libx86/BulletDynamics.lib")
#pragma comment (lib, "Engine/Bullet/libx86/BulletCollision.lib")
#pragma comment (lib, "Engine/Bullet/libx86/LinearMath.lib")
#endif


// Constructors =================================
ModulePhysics3D::ModulePhysics3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "module_physics";

	debug = false;

	collision_conf = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_conf);
	broad_phase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	debug_draw = new DebugDrawer();
}

// Destructors ==================================
ModulePhysics3D::~ModulePhysics3D()
{
	RELEASE(debug_draw);
	RELEASE(solver);
	RELEASE(broad_phase);
	RELEASE(dispatcher);
	RELEASE(collision_conf);
}

// Game Loop ====================================
// Render not available yet
bool ModulePhysics3D::Init()
{
	LOG("Creating 3D Physics simulation");
	bool ret = true;

	return ret;
}

// ---------------------------------------------------------
bool ModulePhysics3D::Start()
{
	LOG("Creating Physics environment");

	world = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_conf);
	world->setDebugDrawer(debug_draw);
	world->setGravity(GRAVITY);

	// Big rectangle as ground
	/*{
		btCollisionShape* colShape = new btBoxShape(btVector3(10.0f, 2.0f, 10.0f));
		btDefaultMotionState* myMotionState = new btDefaultMotionState();
		btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, myMotionState, colShape);
		btRigidBody* body = new btRigidBody(rbInfo);
		
		world->addRigidBody(body);
	}*/

	return true;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PreUpdate(float dt)
{
	world->stepSimulation(dt, 15);

	// TODO 6: Detect collisions:
	uint manifolds_num = world->getDispatcher()->getNumManifolds();
	// - Iterate all manifolds
	for (uint k = 0; k < manifolds_num; k++) 
	{
		// - Count the number of contacts
		btPersistentManifold* current_mainfold = world->getDispatcher()->getManifoldByIndexInternal(k);
		if ((current_mainfold->getNumContacts()) > 0)
		{
			// - If we have contacts, get both PhysBody3D from userpointers
			PhysBody3D* bodyA =  ((PhysBody3D*)current_mainfold->getBody0()->getUserPointer());
			PhysBody3D* bodyB = ((PhysBody3D*)current_mainfold->getBody1()->getUserPointer());
			// - If iterate all contact listeners and call them
			if (bodyA != NULL && bodyB != NULL)
			{
				for (std::list<Module*>::iterator it = bodyA->collision_listeners.begin(); it != bodyA->collision_listeners.end(); it++)
				{
					(*it)->OnCollision(bodyA, bodyB);
				}

				for (std::list<Module*>::iterator it = bodyB->collision_listeners.begin(); it != bodyB->collision_listeners.end(); it++)
				{
					(*it)->OnCollision(bodyA, bodyB);
				}

			}
		}
	}
	

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::Update(float dt)
{

	//Active/Desactive collide body debug mode
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;



	if(debug)
	{
		world->debugDrawWorld();

		/*// Render vehicles
		p2List_item<PhysVehicle3D*>* item = vehicles.getFirst();
		while (item)
		{
			item->data->Render();
			item = item->next;
		}*/

		//Create a Sphere with debug mode
		if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		{
			Sphere s(1);
			s.SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
			AddBody(((Primitive*)&s), DINAMIC_SPHERE);
		}

		//Create a Cube with debug mode
		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		{
			Cube c(0.5f,0.5f,0.5f);
			c.SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
			AddBody(((Primitive*)&c), DINAMIC_CUBE);
		}

		//Create a Plane with debug mode
		if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
		{
			Plane p(0, 1.0f, 0,0);
			p.SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
			AddBody(((Primitive*)&p), DINAMIC_PLANE);
		}

		//Create a Cylinder with debug mode
		if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
		{
			Cylinder c(1.0f, 0.5f);
			c.SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
			AddBody(((Primitive*)&c), DINAMIC_CYLINDER);
		}

	}




	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics3D::CleanUp()
{
	LOG("Destroying 3D Physics simulation");

	// Remove from the world all collision bodies
	for(int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		world->removeCollisionObject(obj);
	}

	
	//Free the shapes
	for (std::list<btCollisionShape*>::iterator it = shapes.begin(); it != shapes.end(); it++)
	{
		RELEASE((*it));
	}
	shapes.clear();
	LOG("Shapes Cleared!\n");

	//Free the bodies
	for (std::list<PhysBody3D*>::iterator it = bodies.begin(); it != bodies.end(); it++)
	{
		RELEASE((*it));
	}
	bodies.clear();
	LOG("Bodies Cleared!\n");

	RELEASE(world);

	return true;
}

// Functionality ================================
PhysBody3D * ModulePhysics3D::AddBody(const Primitive* primitive, OBJECT_TYPE object_type, float mass, BODY_MASK I_am)
{
	btCollisionShape* colShape = nullptr;

	switch (object_type)
	{
	case DINAMIC_CUBE:		case STATIC_CUBE:		case SENSOR_CUBE:		colShape = new btBoxShape(btVector3(((Cube*)primitive)->size.x / 2.0f, ((Cube*)primitive)->size.y / 2.0f, ((Cube*)primitive)->size.z / 2.0f));								break;
	case DINAMIC_CYLINDER:	case STATIC_CYLINDER:	case SENSOR_CYLINDER:	colShape = new btCylinderShape(btVector3(((Cylinder*)primitive)->radius, ((Cylinder*)primitive)->height, ((Cylinder*)primitive)->radius));									break;
	case DINAMIC_PLANE:		case STATIC_PLANE:		case SENSOR_PLANE:		colShape = new btStaticPlaneShape(btVector3(((Plane*)primitive)->normal.x, ((Plane*)primitive)->normal.y, ((Plane*)primitive)->normal.z), ((Plane*)primitive)->constant);	break;
	case DINAMIC_SPHERE:	case STATIC_SPHERE:		case SENSOR_SPHERE:		colShape = new btSphereShape(((Sphere*)primitive)->radius);		break;
	}

	//Set body transform matrix
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&primitive->transform);
	//Calculate and set inertia of object have mass
	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	//Set the body motion state (used later to interpolate and sync body transforms)
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	//Create and store the body
	btRigidBody* body = new btRigidBody(rbInfo);


	// ======
	body->setRestitution(1.0f);
	
	if (object_type == STATIC_CUBE || object_type == STATIC_SPHERE || object_type == STATIC_CYLINDER || object_type == STATIC_PLANE ||
		object_type == SENSOR_CUBE || object_type == SENSOR_SPHERE || object_type == SENSOR_CYLINDER || object_type == SENSOR_PLANE) {
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		body->setMassProps(0, btVector3(0, 0, 0));
	}

	// ======
/*
	int collide_with = 0;
	switch (I_am)
	{
	case CAR:
		collide_with = MAP | SENSOR;
		break;

	case MAP:
		collide_with = CAR;
		break;

	case SENSOR:
		collide_with = CAR;
		break;
	}
	*/

	PhysBody3D* pbody = new PhysBody3D(body);
	if (object_type == SENSOR_CUBE || object_type == SENSOR_SPHERE || object_type == SENSOR_CYLINDER || object_type == SENSOR_PLANE)pbody->SetAsSensor(true);
	bodies.push_back(pbody);
	//Put body pointers in user & world data
	body->setUserPointer(pbody);
	world->addRigidBody(body);


	return pbody;
}

btPoint2PointConstraint* ModulePhysics3D::Add_P2P_Constraint(btRigidBody& rbA, btRigidBody& rbB, const btVector3& pivotInA, const btVector3& pivotInB)
{

	btPoint2PointConstraint* constrain = new btPoint2PointConstraint(rbA, rbB, pivotInA, pivotInB);
	world->addConstraint(constrain);
	constrains.push_back(constrain);
	return constrain;
}

btHingeConstraint* ModulePhysics3D::Add_Hinge_Constraint(btRigidBody & rbA, btRigidBody & rbB, const btVector3& pivotInA, const btVector3& pivotInB, btVector3& axisInA, btVector3& axisInB)
{
	btHingeConstraint* constrain = new btHingeConstraint(rbA, rbB, pivotInA, pivotInB, axisInA, axisInB);
	world->addConstraint(constrain);
	constrains.push_back(constrain);
	return constrain;
}

btHingeConstraint * ModulePhysics3D::Add_EnginedHinge_Constraint(btRigidBody & rbA, btRigidBody & rbB, const btVector3 & pivotInA, const btVector3 & pivotInB, btVector3 & axisInA, btVector3 & axisInB)
{
	btHingeConstraint* constrain = new btHingeConstraint(rbA, rbB, pivotInA, pivotInB, axisInA, axisInB);
	constrain->enableAngularMotor(true, 4, 100); 
	//constrain-
	constrain->setDbgDrawSize(2.0f);
	world->addConstraint(constrain, true);
	constrains.push_back(constrain);
	return constrain;
}

btFixedConstraint * ModulePhysics3D::Add_Fixed_Constraint(btRigidBody & rbA, btRigidBody & rbB, const btTransform & frameA, const btTransform & frameB)
{

	btFixedConstraint* constrain = new btFixedConstraint(rbA, rbB, frameA, frameB);
	world->addConstraint(constrain);
	constrains.push_back(constrain);
	return constrain;
}



/// DebugDrawer ---------------------------------
// Constructors =================================
DebugDrawer::DebugDrawer() : line(0, 0, 0)
{

}

// Functionality ================================
void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	line.origin.Set(from.getX(), from.getY(), from.getZ());
	line.destination.Set(to.getX(), to.getY(), to.getZ());
	line.color.Set(color.getX(), color.getY(), color.getZ());
	line.Render();
}

void DebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	point.transform.translate(PointOnB.getX(), PointOnB.getY(), PointOnB.getZ());
	point.color.Set(color.getX(), color.getY(), color.getZ());
	point.Render();
}

void DebugDrawer::reportErrorWarning(const char* warningString)
{
	LOG("Bullet warning: %s", warningString);
}

void DebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
	LOG("Bullet draw text: %s", textString);
}

void DebugDrawer::setDebugMode(int debugMode)
{
	mode = (DebugDrawModes) debugMode;
}

int	 DebugDrawer::getDebugMode() const
{
	return mode;
}
/// ---------------------------------------------