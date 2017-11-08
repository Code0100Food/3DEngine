#include "ResourcesManager.h"

#include "CubeGenerator.h"
#include "SphereGenerator.h"
#include "CylinderGenerator.h"

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
	//Generate all the default mesh resources for the primitives

	//Used to generate primitives 
	std::pair<std::vector<uint>, std::vector<Vertex>> geometry_data;

	//Cube --------------------------------------
	//Generate the cube logic
	math::AABB cube_logic;
	cube_logic.minPoint = math::float3(0, 0, 0);
	cube_logic.maxPoint = math::float3(1, 1, 1);
	CubeGenerator cube;
	cube.SetGeometry(cube_logic);
	cube.SetDivisions(2);
	geometry_data = cube.Generate();

	//Generate the cube mesh
	cube_mesh = (ResourceMesh*)CreateResource(RESOURCE_TYPE::MESH_RESOURCE);
	cube_mesh->SetOwnFile("Predefined_Cube_Mesh");
	cube_mesh->SetOriginalFile("Predefined_Cube_Mesh");
	cube_mesh->SetIndices(geometry_data.first);
	cube_mesh->SetVertices(geometry_data.second);
	cube_mesh->SetupMesh();

	geometry_data.first.clear();
	geometry_data.second.clear();

	//Sphere Low --------------------------------
	//Generate the sphere logic
	math::Sphere sphere_logic;
	sphere_logic.r = 1;
	sphere_logic.pos = math::float3(0, 0, 0);
	SphereGenerator sphere;
	sphere.SetGeometry(sphere_logic);
	sphere.SetDivisions(2);
	geometry_data = sphere.Generate();

	sphere_low_mesh = (ResourceMesh*)CreateResource(RESOURCE_TYPE::MESH_RESOURCE);
	sphere_low_mesh->SetOwnFile("Predefined_Sphere_Low_Mesh");
	sphere_low_mesh->SetOriginalFile("Predefined_Sphere_Low_Mesh");
	sphere_low_mesh->SetIndices(geometry_data.first);
	sphere_low_mesh->SetVertices(geometry_data.second);
	sphere_low_mesh->SetupMesh();

	geometry_data.first.clear();
	geometry_data.second.clear();

	//Sphere Hi ---------------------------------
	//Generate the sphere logic
	sphere_logic.r = 1;
	sphere_logic.pos = math::float3(0, 0, 0);
	sphere.SetGeometry(sphere_logic);
	sphere.SetDivisions(3);
	geometry_data = sphere.Generate();

	sphere_hi_mesh = (ResourceMesh*)CreateResource(RESOURCE_TYPE::MESH_RESOURCE);
	sphere_hi_mesh->SetOwnFile("Predefined_Sphere_Hi_Mesh");
	sphere_hi_mesh->SetOriginalFile("Predefined_Sphere_Hi_Mesh");
	sphere_hi_mesh->SetIndices(geometry_data.first);
	sphere_hi_mesh->SetVertices(geometry_data.second);
	sphere_hi_mesh->SetupMesh();

	geometry_data.first.clear();
	geometry_data.second.clear();

	//Cylinder Low ------------------------------
	//Generate the cylinder logic
	math::Cylinder cylinder_logic;
	cylinder_logic.r = 1;
	cylinder_logic.l.b = math::float3(0, 0, 0);
	cylinder_logic.l.a = math::float3(0, 1, 0);
	CylinderGenerator cylinder;
	cylinder.SetGeometry(cylinder_logic);
	cylinder.SetDivisions(6);
	geometry_data = cylinder.Generate();

	cylinder_low_mesh = (ResourceMesh*)CreateResource(RESOURCE_TYPE::MESH_RESOURCE);
	cylinder_low_mesh->SetOwnFile("Predefined_Cylinder_Low_Mesh");
	cylinder_low_mesh->SetOriginalFile("Predefined_Cylinder_Low_Mesh");
	cylinder_low_mesh->SetIndices(geometry_data.first);
	cylinder_low_mesh->SetVertices(geometry_data.second);
	cylinder_low_mesh->SetupMesh();

	geometry_data.first.clear();
	geometry_data.second.clear();

	//Cylinder Hi -------------------------------
	//Generate the cylinder logic
	cylinder_logic.r = 1;
	cylinder_logic.l.b = math::float3(0, 0, 0);
	cylinder_logic.l.a = math::float3(0, 1, 0);
	cylinder.SetGeometry(cylinder_logic);
	cylinder.SetDivisions(12);
	geometry_data = cylinder.Generate();

	cylinder_hi_mesh = (ResourceMesh*)CreateResource(RESOURCE_TYPE::MESH_RESOURCE);
	cylinder_hi_mesh->SetOwnFile("Predefined_Cylinder_Hi_Mesh");
	cylinder_hi_mesh->SetOriginalFile("Predefined_Cylinder_Hi_Mesh");
	cylinder_hi_mesh->SetIndices(geometry_data.first);
	cylinder_hi_mesh->SetVertices(geometry_data.second);
	cylinder_hi_mesh->SetupMesh();

	geometry_data.first.clear();
	geometry_data.second.clear();

	return true;
}

bool ResourcesManager::CleanUp()
{
	//Release all the resources
	std::map<uint, Resource*>::iterator res_item = resources.begin();
	while (res_item != resources.end())
	{
		RELEASE(res_item._Ptr->_Myval.second);
		res_item++;
	}
	resources.clear();

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
		resources.insert(std::pair<uint, Resource*>(new_res->GetID(), new_res));
	}
	
	return new_res;
}

ResourceMesh * ResourcesManager::GetPrimitiveResourceMesh(PRIMITIVE_TYPE type)
{
	switch (type)
	{
	case UNDEF_PRIMITIVE:
		break;
	case PRIMITIVE_CUBE:			return cube_mesh;
	case PRIMITIVE_SPHERE:			return sphere_low_mesh;
	case PRIMITIVE_SPHERE_HI:		return sphere_hi_mesh;
	case PRIMITIVE_CYLINDER:		return cylinder_low_mesh;
	case PRIMITIVE_CYLINDER_HI:		return cylinder_hi_mesh;
	case PRIMITIVE_CAPSULE:
		break;
	case PRIMITIVE_FRUSTUM:
		break;
	case PRIMITIVE_GRID:
		break;
	}

	return nullptr;
}
