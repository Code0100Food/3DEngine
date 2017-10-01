#include "GeometryManager.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

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
	//Enable the vertex and elements flags
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);

	std::list<Primitive_*>::const_iterator geom = geometry_list.begin();
	while (geom != geometry_list.end())
	{
		geom._Ptr->_Myval->Draw();

		geom++;
	}

	//Reset the buffers focus
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Disable the vertex and elements flags
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_ELEMENT_ARRAY_BUFFER);

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
Primitive_* GeometryManager::CreatePrimitive(PRIMITIVE_TYPE type)
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
		new_primitive = new Sphere_();
		break;
	case PRIMITIVE_CYLINDER:
		new_primitive = new Cylinder_();
		break;
	case PRIMITIVE_RAY:
		break;
	case PRIMITIVE_CAPSULE:
		new_primitive = new Capsule_();
		break;
	case PRIMITIVE_FRUSTUM:
		break;
	}

	//Add the new primitive at the geometry list
	geometry_list.push_back(new_primitive);

	return new_primitive;
}