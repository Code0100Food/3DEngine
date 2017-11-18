#ifndef _RESOURCES_MANAGER_
#define _RESOURCES_MANAGER_

#include "Module.h"
#include "Primitive_.h"
#include <map>

#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"

#include"Timer.h"
#include <experimental/filesystem>

#define METAS_UPDATE_RATE 5000

class ResourcesManager : public Module
{
public:

	ResourcesManager(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~ResourcesManager();

public:

	bool			Start();
	update_status	Update(float dt);
	bool			CleanUp();

private:

	std::map<uint, Resource*> resources;

	//Primitives resources
	ResourceMesh* cube_mesh = nullptr;
	ResourceMesh* sphere_low_mesh = nullptr;
	ResourceMesh* sphere_hi_mesh = nullptr;
	ResourceMesh* cylinder_low_mesh = nullptr;
	ResourceMesh* cylinder_hi_mesh = nullptr;

	//Update resources
	Timer metas_timer;
	std::experimental::filesystem::path path;
	std::experimental::filesystem::v1::file_time_type real_last_time;
	std::time_t last_time;

public:

	//Resource management methods
	Resource*		CreateResource(RESOURCE_TYPE type, uint id = 0);
	ResourceMesh*	GetPrimitiveResourceMesh(PRIMITIVE_TYPE type);

	//Import methods
	uint	CheckAssetsResources();
	bool	CheckIfFileIsImported(const char* path)const;

	//Find Methods
	Resource*	Find(const char* file_path, RESOURCE_TYPE type)const;
	Resource*	Find(uint id)const;
	uint		FindMetaFile(const char* own_file_path)const;

	//Metas Methods
	void		LoadMetaFiles();
	void		UpdateMetaFiles();

	//UI
	void		BlitConfigInfo();
	Resource*	BlitResourceButtonsByType(RESOURCE_TYPE type);
};
#endif // !_RESOURCES_MANAGER_
