#include "GeometryManager.h"


// Constructors =================================
GeometryManager::GeometryManager(const char * _name, MODULE_ID _id, bool _config_menu, bool _enabled) :Module(_name, _id, _enabled)
{

}

// Destructors ==================================
GeometryManager::~GeometryManager()
{

}

// Game Loop ====================================
bool GeometryManager::Draw()
{
	std::list<Primitive_*>::const_iterator geom = geometry_list.begin();
	while (geom != geometry_list.end())
	{
		geom._Ptr->_Myval->Draw();

		geom++;
	}

	return true;
}

bool GeometryManager::CleanUp()
{
	std::list<Primitive_*>::const_iterator geom = geometry_list.begin();
	while (geom != geometry_list.end())
	{
		RELEASE(geom._Ptr->_Myval);

		geom++;
	}
	geometry_list.clear();

	return true;
}

// Functionality ================================
Primitive_* GeometryManager::CreatePrimitive(PRIMITIVE_TYPE type, ...)
{
	Primitive_* new_primitive = nullptr;

	switch (type)
	{
	case PRIMITIVE_POINT:
		break;
	case PRIMITIVE_LINE:
		break;
	case PRIMITIVE_PLANE:
		break;
	case PRIMITIVE_CUBE:
		new_primitive = new Cube_();
		break;
	case PRIMITIVE_SPHERE:
		break;
	case PRIMITIVE_CYLINDER:
		break;
	case PRIMITIVE_RAY:
		break;
	case PRIMITIVE_CAPSULE:
		break;
	case PRIMITIVE_FRUSTUM:
		break;
	}

	//Add the new primitive at the geometry list
	geometry_list.push_back(new_primitive);

	return new_primitive;
}