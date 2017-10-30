#ifndef _GEOMETRY_MANAGER_H
#define _GEOMETRY_MANAGER_H

#include "Module.h"

#include "Primitive_.h"
#include "Capsule_.h"
#include "Frustrum_.h"
#include "Grid_.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib,"Engine/Assimp/libx86/assimp.lib")

class GeometryManager : public Module
{

	friend class GameObject;
	friend class ComponentMeshRenderer;

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

	//Ground grid
	Grid_*	grid = nullptr; 

	// Config data
	bool	show_grid = true;

	float	mesh_lines_width = 1.0f;
	float	mesh_color[4];
	float	vertex_normals_color[4];
	float	face_normals_color[4];
	float	bounding_box_color[4];

	float	primitive_lines_width = 1.0f;
	float	primitive_color[4];

};

#endif // !_GEOMETRY_MANAGER_H
