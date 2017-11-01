#ifndef _MODULE_CAMERA_3D_H_
#define _MODULE_CAMERA_3D_H_

#include "Module.h"
#include "Globals.h"

#include "MathGeoLib/Geometry/Frustum.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/Quat.h"

#define CAMERA_DIST_ON_FOCUS 0.6

struct PhysVehicle3D;
class GameObject;

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

private:

	float			camera_z_mov_vel = 0.0f;
	float			rot_around_vel = 0.0f;
	float			strafe_vel = 0.0f;
	float			wasd_vel = 0.0f;

	math::Frustum	editor_camera_frustrum;
	math::float3	focus_point;

	float			angle_to_rotate;
	math::Quat		rotate_quaternion;
	float			camera_dist = 0;

public:

	void	LookAt(const math::float3 &Spot);
	void	LookAtGameObject(GameObject* obj);
	void	Look(const math::float3& look_here, bool RotateAroundLookingPoint);

	float*			GetViewMatrix() const;
	float*			GetViewMatrixTransposed() const;
	float*			GetProjectionMatrix() const;
	float*			GetProjectionMatrixTransposed() const;
	float			GetFrustrumNearPlaneDistance() const;
	float			GetFrustrumFarPlaneDistance() const;
	math::float3	GetPosition() const;

};


#endif // !_MODULE_CAMERA_3D_H_

