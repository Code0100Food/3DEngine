#ifndef _GEOMETRY_MANAGER_H
#define _GEOMETRY_MANAGER_H

#include "Module.h"

#include "Primitive_.h"
#include "Cube_.h"
#include "Sphere_.h"
#include "Capsule_.h"
#include "Cylinder_.h"
#include "Frustrum_.h"
#include "Mesh_.h"
#include "Model_.h"
#include "Grid_.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib,"Engine/Assimp/libx86/assimp.lib")

class GeometryManager : public Module
{

	friend class Mesh_;
	friend class Model_;
	friend class ComponentMesh;

public:

	GeometryManager(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~GeometryManager();

public:

	bool Awake(const JSON_Object * data_root);
	bool Start()final;
	bool Draw();
	bool CleanUp()final;
	void BlitConfigInfo()final;
	void SaveConfigInfo(JSON_Object* data_root)final;

protected:

	std::list<Primitive_*>	primitives_list;
	std::list<Model_*>		models_list;

	//Ground grid
	Grid_*	grid = nullptr; 

	// Config data
	bool	show_grid = true;

	bool	show_meshes = true;
	float	mesh_lines_width = 1.0f;
	float	mesh_color[4];
	float	vertex_normals_color[4];
	float	face_normals_color[4];
	float	bounding_box_color[4];

	bool	show_primitives = true;
	float	primitive_lines_width = 1.0f;
	float	primitive_color[4];

	//Scene objects window data
	bool	show_scene_objects = true;

public:

	//Temp Method
	Model_*	GetSelectedModel()const;

	//Objects window methods
	void ShowSceneObjects();
	bool GetObjWindowState()const;
	void BlitObjectsWindow();

	//Geometry factory ------
	Primitive_* CreatePrimitive(PRIMITIVE_TYPE type, bool push_at_list = true);
	bool		LoadScene(const char* folder);

};

#endif // !_GEOMETRY_MANAGER_H
