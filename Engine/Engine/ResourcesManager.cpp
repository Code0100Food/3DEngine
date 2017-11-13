#include "ResourcesManager.h"

#include "CubeGenerator.h"
#include "SphereGenerator.h"
#include "CylinderGenerator.h"

#include "Application.h"
#include "FileSystem.h"
#include "ImporterManager.h"
#include "Serializer.h"
#include "ModuleTextures.h"

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

	//Check the user assets ---------------------
	LOG("Assets Updated with %i changes!", CheckAssetsResources());

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
	case MESH_RESOURCE:			new_res = (Resource*)new ResourceMesh();		break;
	case MATERIAL_RESOURCE:		new_res = (Resource*)new ResourceMaterial();	break;
	case SCENE_RESOURCE:		break;
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

bool ResourcesManager::ImportFile(const char * path, bool put_on_scene)
{
	bool b_ret = false;
	bool on_scene = put_on_scene;

	//Get the file format to call the correct importer
	std::string format;
	App->fs->GetFileFormatFromPath(path, &format);
	IMPORT_TYPE imp_type = App->importer->GetImportTypeFromFormat(format.c_str());
	Resource* new_resource = nullptr;

	std::string n_path = path;
	int ret = -1;
	if (!App->fs->IsInAssets(path))
	{
		ret = App->fs->CloneFile(path, App->fs->GetAssetsFolder(), &n_path);
	}
	else if (!CheckIfFileIsImported(path)) ret = 1;
	else ret = 0;

	if(ret == -1) //Error on file read case
	{
		LOG("[error] Error Importing [%s]", path);
		return false;
	}

	else if (ret == 0) //File is in assets and imported but lets check if it's the actual version
	{
		LOG("File is already imported!");
		//If the file already exists in assets lets update the content!
		new_resource = Find(path);
	}

	else if (ret == 1) //File is not imported case
	{
		/*Import the file*/



		switch (imp_type)
		{
		case UNDEF_IMPORT:
			LOG("[error] File format not supported!");
			on_scene = false;
			break;
		case MATERIAL_IMPORT:
			new_resource = App->res_manager->CreateResource(RESOURCE_TYPE::MATERIAL_RESOURCE);
			b_ret = App->importer->material_importer.Import(n_path.c_str(), (ResourceMaterial*)new_resource);
			b_ret = App->importer->material_importer.Load((ResourceMaterial*)new_resource);
			break;
		case MESH_IMPORT:
			break;
		case MODEL_IMPORT:
			b_ret = App->importer->model_importer.Import(path);
			break;
		}

		if (new_resource != nullptr)
		{
			//Generate a meta file to link the generated resource with the file data
			Serializer meta_file;

			bool ret = new_resource->Save(meta_file);

			if (ret)
			{
				//Save the generated meta file
				char* buffer = nullptr;
				uint size = meta_file.Save(&buffer);
				char meta_name[200];
				sprintf(meta_name, "%s.meta", new_resource->GetOwnFile());
				App->fs->SaveFile(meta_name, buffer, size - 1, LIBRARY_META_FOLDER);
				
				RELEASE_ARRAY(buffer);
			}
			else on_scene = false;
		}
	}

	if (on_scene)
	{
		switch (imp_type)
		{
		case MATERIAL_IMPORT:
			if(new_resource != nullptr)App->textures->SetCustomTexutreID(((ResourceMaterial*)new_resource)->GetMaterialID());
			break;
		case MESH_IMPORT:
			break;
		case MODEL_IMPORT:
			/*Here we generate model objects and link them*/
			//b_ret = App->importer->model_importer.Load(path);
			break;
		}
	}
	
	return b_ret;
}

uint ResourcesManager::CheckAssetsResources()
{
	return App->fs->GetUserRootDir()->ImportAllFilesInside();
}

bool ResourcesManager::CheckIfFileIsImported(const char * path) const
{
	for (map<uint, Resource*>::const_iterator res = resources.begin(); res != resources.end(); res++)
	{
		if (strcmp(res->second->GetOriginalFile(),path) == 0)return true;
	}
	return false;
}

Resource * ResourcesManager::Find(const char * file_path) const
{
	for (map<uint, Resource*>::const_iterator res = resources.begin(); res != resources.end(); res++)
	{
		if (strcmp(res->second->GetOriginalFile(), file_path) == 0)return res._Ptr->_Myval.second;
	}
	return nullptr;
}

void ResourcesManager::BlitConfigInfo()
{
	for (map<uint, Resource*>::const_iterator res = resources.begin(); res != resources.end(); res++)
	{
		ImGui::Text(res->second->GetOwnFile());
		ImGui::Text("References: %i", res->second->GetReferences());
	}
}
