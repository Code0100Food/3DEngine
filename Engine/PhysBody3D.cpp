#include "PhysBody3D.h"
#include "glmath.h"
#include "Engine/Bullet/include/btBulletDynamicsCommon.h"


// ---------------------------------------------------------
PhysBody3D::PhysBody3D(btRigidBody* body):body(body)
{
	body->setUserPointer(this);
}

// ---------------------------------------------------------
PhysBody3D::~PhysBody3D()
{}

void PhysBody3D::Push(float x, float y, float z)
{
	body->applyCentralImpulse(btVector3(x, y, z));
}

// ---------------------------------------------------------
void PhysBody3D::GetTransform(float* matrix) const
{
	//Get the PhysBody3D transformation matrix
	if (body != NULL && matrix != NULL) {
		body->getWorldTransform().getOpenGLMatrix(matrix);
	}
}

// ---------------------------------------------------------
void PhysBody3D::SetTransform(const float* matrix) const
{
	//Set the PhysBody3D transformation matrix
	if (body != NULL && matrix != NULL) {
		btTransform transform;
		transform.setFromOpenGLMatrix(matrix);
		body->setWorldTransform(transform);
	}
}

// ---------------------------------------------------------
void PhysBody3D::SetPos(float x, float y, float z)
{
	//Set the position of the body
	if (body != NULL) {
		btTransform trans = body->getWorldTransform();
		trans.setOrigin(btVector3(x, y, z));
		body->setWorldTransform(trans);
	}
}

vec3 PhysBody3D::GetPos() const
{
	if (body != NULL) {
		mat4x4 matrix;
		body->getWorldTransform().getOpenGLMatrix(&matrix);
		return vec3(matrix[12], matrix[13], matrix[14]);
	}
	else return vec3(0, 0, 0);
}

btRigidBody* PhysBody3D::get_rigid_body() const
{
	return body;
}


void PhysBody3D::SetAsSensor(bool is_sensor)
{
	if (this->is_sensor != is_sensor)
	{
		this->is_sensor = is_sensor;
		if (is_sensor == true)
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		else
			body->setCollisionFlags(body->getCollisionFlags() &~btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
}

bool PhysBody3D::IsSensor() const
{
	return is_sensor;
}

