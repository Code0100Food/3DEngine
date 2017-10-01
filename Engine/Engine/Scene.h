#ifndef _SCENE_H_
#define _SCENE_H_

#include "Module.h"
#include "MathGeoLib/Algorithm/Random/LCG.h"
#include "MathGeoLib/Geometry/GeometryAll.h"
#include "Primitive.h"

class Cube_;
class Sphere_;
class Capsule_;
class Cylinder_;

class Scene : public Module
{
public:

	Scene(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~Scene();

public:

	bool			Start() final;
	update_status	Update(float dt)final;
	bool			CleanUp();

public: 

	//Basic Plane
	_Plane grid;

	//Windows management
	bool random_window = false;
	bool maths_window = false;

	//Random Screen
	math::LCG*	rand_num_generator = nullptr;
	int			val_a = 0;
	int			val_b = 0;
	int			random_int = 0;
	float		random_float = 0;

	// Primitives Screen
	//Vector
	math::Line line_a;
	math::Line line_b;

	float	   dot_product = 0;
	math::Line cross_product;

	//Spheres
	bool sphere_1 = false;
	bool capsule_1 = false;
	bool aabb_1 = false;
	bool triangle_1 = false;
	bool plane_1 = false;

	bool sphere_2 = false;
	bool capsule_2 = false;
	bool aabb_2 = false;
	bool triangle_2 = false;
	bool plane_2 = false;

	math::Sphere sphere_a;
	math::Sphere sphere_b;

	math::Capsule capsule_a;
	math::Capsule capsule_b;

	math::AABB box_a;
	math::AABB box_b;

	math::Triangle tri_a;
	math::Triangle tri_b;

	math::Plane plane_a;
	math::Plane plane_b;

	std::string collide_test;
	
	//Geometry
	Cube_*		cube = nullptr;
	Sphere_*	sphere = nullptr;
	Cylinder_*	cylinder = nullptr;

	Capsule_*	capsule = nullptr;


public:

	void ShowRandom();
	void ShowMaths();

};
#endif
