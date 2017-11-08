#include "ResourcesManager.h"

// Constructors =================================
ResourcesManager::ResourcesManager(const char * _name, MODULE_ID _id, bool _config_menu, bool _enabled) :Module(_name, _id, _config_menu, _enabled)
{

}

// Destructors ==================================
ResourcesManager::~ResourcesManager()
{

}

bool ResourcesManager::Start()
{
	return true;
}

Resource * ResourcesManager::CreateResource(RESOURCE_TYPE type)
{
	Resource* new_res = nullptr;

	//Generate the new resource
	switch (type)
	{
	case MESH_RESOURCE: new_res = (Resource*)new ResourceMesh();	break;
	case MATERIAL_RESOURCE:
		break;
	case SCENE_RESOURCE:
		break;
	}

	//Add it to the map
	if (new_res != nullptr)
	{
		resources[new_res->GetID()] = new_res;
	}
	
	return new_res;
}
