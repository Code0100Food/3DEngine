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

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib,"Engine/Assimp/libx86/assimp.lib")

class GeometryManager : public Module
{
public:

	GeometryManager(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~GeometryManager();

public:

	bool Start()final;
	bool Draw();
	bool CleanUp()final;

private:

	std::list<Primitive_*> geometry_list;

public:

	//Geometry factory ------
	
	/*
	- Cube: float3 min vertex, float3 max vertex
	*/
	Primitive_* CreatePrimitive(PRIMITIVE_TYPE type);
	
	bool		LoadScene(const char* folder);

};

#endif // !_GEOMETRY_MANAGER_H
