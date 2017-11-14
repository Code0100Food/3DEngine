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
	cube_mesh->SetConstInMemory();

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
	sphere_low_mesh->SetConstInMemory();

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
	sphere_hi_mesh->SetConstInMemory();

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
	cylinder_low_mesh->SetConstInMemory();

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
	cylinder_hi_mesh->SetConstInMemory();

	geometry_data.first.clear();
	geometry_data.second.clear();

	//Load all the resources meta files ---------
	LoadMetaFiles();

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

bool ResourcesManager::ImportFile(const char * path)
{
	if (Find(path) != nullptr)return true; //If is already imported!

	bool b_ret = false;

	//Get the file format to call the correct importer
	std::string format;
	App->fs->GetFileFormatFromPath(path, &format);
	IMPORT_TYPE imp_type = App->importer->GetImportTypeFromFormat(format.c_str());
	Resource* new_resource = nullptr;

	std::string n_path = path;
	if (!App->fs->IsInAssets(path))
	{
		if (App->fs->CloneFile(path, App->fs->GetAssetsFolder(), &n_path) == -1)
		{
			LOG("[error] Error Importing [%s]", path);
			return false;
		}
	}

	switch (imp_type)
	{
	case UNDEF_IMPORT:
		LOG("[error] File format not supported!");
		break;
	case MATERIAL_IMPORT:
		new_resource = App->res_manager->CreateResource(RESOURCE_TYPE::MATERIAL_RESOURCE);
		b_ret = App->importer->material_importer.Import(n_path.c_str(), (ResourceMaterial*)new_resource);
		break;
	case MESH_IMPORT:
		break;
	case MODEL_IMPORT:
		b_ret = App->importer->model_importer.Import(path);
		break;
	}

	if (new_resource != nullptr)
	{
		char meta_name[200];
		sprintf(meta_name, "%s.meta", new_resource->GetOwnFile());
		uint meta_id = this->FindMetaFile(meta_name);
			
		if (meta_id != 0)new_resource->SetID(meta_id);
		else
		{
			//Generate a meta file to link the generated resource with the file data
			Serializer meta_file;

			bool ret = new_resource->Save(meta_file);

			if (ret)
			{
				//Save the generated meta file
				char* buffer = nullptr;
				uint size = meta_file.Save(&buffer);
				App->fs->SaveFile(meta_name, buffer, size - 1, LIBRARY_META_FOLDER);

				RELEASE_ARRAY(buffer);
			}
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

uint ResourcesManager::FindMetaFile(const char * own_file_path) const
{
	//Look for the meta inside the metas folder
	if (App->fs->GetMetasDir()->FindFile(own_file_path))
	{
		//Load meta and get the ID
		char str[150];
		sprintf(str, "%s%s", LIBRARY_META_FOLDER, own_file_path);
		const JSON_Value* data = json_parse_file(str);
		if (data == nullptr)
		{
			LOG("[error] Meta file %s found but error on load!", own_file_path);
			return 0;
		}

		//Serializer whit all the new scene data
		Serializer file_data(data);

		return file_data.GetInt("id");
	}

	return 0;
}

void ResourcesManager::LoadMetaFiles()
{
	//Set String to look inside Parent folder
	char str[150];
	sprintf(str, "%s\\*.*", App->fs->GetMetasDir()->GetPath());

	//Will recieve all the files list
	WIN32_FIND_DATA files_list;

	//Will handle the list when changing the looked element
	HANDLE file_handle = FindFirstFileA(LPCSTR(str), &files_list);

	if (file_handle == INVALID_HANDLE_VALUE)
	{
		LOG("Error in path");
	}

	DWORD attribute;

	bool still_elements = true;
	while (still_elements)
	{
		if (strcmp(files_list.cFileName,".") == 0 || strcmp(files_list.cFileName,"..") == 0)
		{
			//Jump to the other element
			if (!FindNextFile(file_handle, &files_list))
			{
				still_elements = false;
			}
			continue;
		}

		//Load the meta file
		char str[200];
		sprintf(str, "%s%s", LIBRARY_META_FOLDER, files_list.cFileName);
		const JSON_Value* val = json_parse_file(str);
		if (val == NULL)
		{
			LOG("[error] Error on meta file load!");
		}
		else
		{
			Serializer meta_data(val);

			Resource* new_resource = nullptr;
			RESOURCE_TYPE ty = StrToResourceType(meta_data.GetString("res_type"));
			if (ty != UNDEF_RESOURCE)
			{
				new_resource = CreateResource(ty);
				new_resource->SetID(meta_data.GetInt("id"));
				new_resource->SetOriginalFile(meta_data.GetString("original_file"));
				new_resource->SetOwnFile(meta_data.GetString("own_file"));
			}
		}

		//Jump to the other element
		if (!FindNextFile(file_handle, &files_list))
		{
			still_elements = false;
		}
	}

	FindClose(file_handle);
}

void ResourcesManager::BlitConfigInfo()
{
	for (map<uint, Resource*>::const_iterator res = resources.begin(); res != resources.end(); res++)
	{
		ImGui::Text(res->second->GetOwnFile());
		ImGui::Text("References: %i", res->second->GetReferences());
	}
}

ResourceMesh * ResourcesManager::BlitImportedMeshes() const
{
	for (map<uint, Resource*>::const_iterator res = resources.begin(); res != resources.end(); res++)
	{
		if (res->second->GetResourceType() != RESOURCE_TYPE::MESH_RESOURCE)continue;

		if (ImGui::Button(res->second->GetOwnFile()))
		{
			return (ResourceMesh*)res->second;
		}
	}
	
	return nullptr;
}
