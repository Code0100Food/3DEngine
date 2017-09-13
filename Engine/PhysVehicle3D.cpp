#include "PhysVehicle3D.h"
#include "Primitive.h"
#include "ModulePlayer.h"
#include "Application.h"
#include "Bullet/include/btBulletDynamicsCommon.h"
#include "Bullet\include\btBulletDynamicsCommon.h"

// ----------------------------------------------------------------------------
VehicleInfo::~VehicleInfo()
{
	//if(wheels != NULL)
		//delete wheels;
}

// ----------------------------------------------------------------------------
PhysVehicle3D::PhysVehicle3D(btRigidBody* body, btRaycastVehicle* vehicle, const VehicleInfo& info) : PhysBody3D(body), vehicle(vehicle), info(info)
{

	chassis = Cube(info.chassis_size.x, 1, info.chassis_size.z);
	chassis.SetColor(Red);
	Cabine = Cube(6, 2, 1);
	Cabine.SetColor(Red);
	print_Back_1 = Cube(1, 4, 3);
	print_Back_1.SetColor(Red);
	print_Back_2 = Cube(1, 4, 3);
	print_Back_2.SetColor(Red);
	roof = Cube(6, 0.25f, 8);
	roof.SetColor(Red);

	Cube back_light_1(1, 1, 0.5f);
	back_light_1.SetColor(Gray);
	Cube back_light_2(1, 1, 0.5f);
	back_light_2.SetColor(Gray);
	Cube front_light_1(1, 1, 1);
	front_light_1.SetColor(Yellow);
	Cube front_light_2(1, 1, 1);
	front_light_2.SetColor(Yellow);
	Cube Up_light_1(2, 1, 1);
	Cube Up_light_2(2, 1, 1);


	vehicle_lights.PushBack(back_light_1);
	vehicle_lights.PushBack(back_light_2);
	vehicle_lights.PushBack(front_light_1);
	vehicle_lights.PushBack(front_light_2);
	vehicle_lights.PushBack(Up_light_1);
	vehicle_lights.PushBack(Up_light_2);
	


	RePlaceVehicle();
}

// ----------------------------------------------------------------------------
PhysVehicle3D::~PhysVehicle3D()
{
	delete vehicle;
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Render()
{
	//Wheels render -------------------
	wheel.color = Black;
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		wheel.radius = info.wheels[0].radius;
		wheel.height = info.wheels[0].width;

		vehicle->updateWheelTransform(i);
		vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(&wheel.transform);

		wheel.Render();
	}

	//Parts transform update ----------
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&chassis.transform);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&Cabine.transform);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&print_Back_1.transform);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&print_Back_2.transform);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&roof.transform);

	uint k = vehicle_lights.Count();
	for(uint h = 0; h < k; h++)
		vehicle->getChassisWorldTransform().getOpenGLMatrix(&vehicle_lights[h].transform);

	btQuaternion q = vehicle->getChassisWorldTransform().getRotation();

	offset = offset.rotate(q.getAxis(), q.getAngle());
	cabine_position = cabine_position.rotate(q.getAxis(), q.getAngle());
	print_Back_1_position = print_Back_1_position.rotate(q.getAxis(), q.getAngle());
	print_Back_2_position = print_Back_2_position.rotate(q.getAxis(), q.getAngle());
	roof_position = roof_position.rotate(q.getAxis(), q.getAngle());
	back_light_1_position = back_light_1_position.rotate(q.getAxis(), q.getAngle());
	back_light_2_position = back_light_2_position.rotate(q.getAxis(), q.getAngle());
	front_light_1_position = front_light_1_position.rotate(q.getAxis(), q.getAngle());
	front_light_2_position = front_light_2_position.rotate(q.getAxis(), q.getAngle());
	Up_light_1_position = Up_light_1_position.rotate(q.getAxis(), q.getAngle());
	Up_light_2_position = Up_light_2_position.rotate(q.getAxis(), q.getAngle());

	chassis.transform.M[12] += offset.getX();
	chassis.transform.M[13] += offset.getY();
	chassis.transform.M[14] += offset.getZ();

	Cabine.transform.M[12] += cabine_position.getX();
	Cabine.transform.M[13] += cabine_position.getY();
	Cabine.transform.M[14] += cabine_position.getZ();

	print_Back_1.transform[12] += print_Back_1_position.getX();
	print_Back_1.transform[13] += print_Back_1_position.getY();
	print_Back_1.transform[14] += print_Back_1_position.getZ();

	print_Back_2.transform[12] += print_Back_2_position.getX();
	print_Back_2.transform[13] += print_Back_2_position.getY();
	print_Back_2.transform[14] += print_Back_2_position.getZ();

	roof.transform[12] += roof_position.getX();
	roof.transform[13] += roof_position.getY();
	roof.transform[14] += roof_position.getZ();

	vehicle_lights[0].transform[12] += back_light_1_position.getX();
	vehicle_lights[0].transform[13] += back_light_1_position.getY();
	vehicle_lights[0].transform[14] += back_light_1_position.getZ();

	vehicle_lights[1].transform[12] += back_light_2_position.getX();
	vehicle_lights[1].transform[13] += back_light_2_position.getY();
	vehicle_lights[1].transform[14] += back_light_2_position.getZ();

	vehicle_lights[2].transform[12] += front_light_1_position.getX();
	vehicle_lights[2].transform[13] += front_light_1_position.getY();
	vehicle_lights[2].transform[14] += front_light_1_position.getZ();

	vehicle_lights[3].transform[12] += front_light_2_position.getX();
	vehicle_lights[3].transform[13] += front_light_2_position.getY();
	vehicle_lights[3].transform[14] += front_light_2_position.getZ();

	vehicle_lights[4].transform[12] += Up_light_1_position.getX();
	vehicle_lights[4].transform[13] += Up_light_1_position.getY();
	vehicle_lights[4].transform[14] += Up_light_1_position.getZ();

	vehicle_lights[5].transform[12] += Up_light_2_position.getX();
	vehicle_lights[5].transform[13] += Up_light_2_position.getY();
	vehicle_lights[5].transform[14] += Up_light_2_position.getZ();

	Cabine.Render();
	print_Back_2.Render();
	print_Back_1.Render();
	roof.Render();
	chassis.Render();

	for (uint h = 0; h < k; h++)
		vehicle_lights[h].Render();
	
	RePlaceVehicle();
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::ApplyEngineForce(float force)
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].drive == true)
		{
			vehicle->applyEngineForce(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Brake(float force)
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].brake == true)
		{
			vehicle->setBrake(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Turn(float degrees)
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].steering == true)
		{
			vehicle->setSteeringValue(degrees, i);
		}
	}
}

// ----------------------------------------------------------------------------
float PhysVehicle3D::GetKmh() const
{
	return vehicle->getCurrentSpeedKmHour();
}

void PhysVehicle3D::RePlaceVehicle()
{
	offset = btVector3(info.chassis_offset.x, 1.5, info.chassis_offset.z);
	cabine_position = btVector3(0, 2.5f, 3.5f);
	print_Back_1_position = btVector3(2.5f, 4, -2.5f);
	print_Back_2_position = btVector3(-2.5f, 4, -2.5f);
	roof_position = btVector3(0, 6.0f, 0);
	back_light_1_position = btVector3(2.5, 2, -4);
	back_light_2_position = btVector3(-2.5, 2, -4);
	front_light_1_position = btVector3(2.4, 2.5, 4);
	front_light_2_position = btVector3(-2.4, 2.5, 4);
	Up_light_1_position = btVector3(-1, 6.5f, 2);
	Up_light_2_position = btVector3(1, 6.5f, 2);
}

void PhysVehicle3D::LocateVehicle(float x, float y, float z)
{

	this->SetPos(x, y, z);
	this->print_Back_1.SetPos(x + print_Back_1_position.getX(), y + print_Back_1_position.getY(), z + print_Back_1_position.getZ());

}
