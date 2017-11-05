#ifndef _MODULE_CAMERA_3D_H_
#define _MODULE_CAMERA_3D_H_

#include "Module.h"
#include "Globals.h"
#include <map>
#include <queue>

#include "MathGeoLib/Geometry/Frustum.h"
#include "MathGeoLib/Geometry/LineSegment.h"
#include "MathGeoLib/Geometry/Triangle.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/Quat.h"
#include "imgui/imgui_dock.h"


#define CAMERA_DIST_ON_FOCUS 1

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
	float			strafe_vel = 0.0f;
	float			wasd_vel = 0.0f;

	DockContext::Dock*				scene_window = nullptr;
	float							mouse_x_normalized = 0.0f;
	float							mouse_y_normalized = 0.0f;

	std::queue<GameObject*>				check_objects;
	std::multimap<float, GameObject*>	objects_to_pick;

public:

	math::Triangle					triangle_to_test;
	math::LineSegment				mouse_picking;
	math::LineSegment				mouse_picking_local_space;

	math::Frustum	editor_camera_frustrum;
	math::float3	focus_point;

	float			angle_to_rotate;
	math::Quat		rotate_quaternion;
	float			camera_dist = 0;

public:

	void	LookAt(const math::float3 &Spot);
	void	LookAtGameObject(GameObject* obj);
	void	Look(const math::float3& look_here, bool RotateAroundLookingPoint);

	void	SetVerticalFov(float angle_in_deg);

	float*			GetViewMatrix() const;
	float*			GetViewMatrixTransposed() const;
	float*			GetProjectionMatrix() const;
	float*			GetProjectionMatrixTransposed() const;
	float			GetFrustrumNearPlaneDistance() const;
	float			GetFrustrumFarPlaneDistance() const;
	math::float3	GetPosition() const;

private:

	void		CheckAllAABB();
	GameObject*	CheckTriangles();

};


#endif // !_MODULE_CAMERA_3D_H_

