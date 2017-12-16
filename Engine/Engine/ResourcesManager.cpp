#include "ResourcesManager.h"

#include "CubeGenerator.h"
#include "SphereGenerator.h"
#include "CylinderGenerator.h"

#include "Application.h"
#include "FileSystem.h"
#include "ImporterManager.h"
#include "Serializer.h"
#include "ModuleTextures.h"
#include "ModuleImgui.h"
#include "imgui/imgui_dock.h"
#include "GameObject.h"
#include "ModuleScene.h"

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
	cube_mesh->SetID(1);
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
	sphere_low_mesh->SetID(2);
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
	sphere_hi_mesh->SetID(3);
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
	cylinder_low_mesh->SetID(4);
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
	cylinder_hi_mesh->SetID(5);
	cylinder_hi_mesh->SetConstInMemory();

	geometry_data.first.clear();
	geometry_data.second.clear();

	//Load all the resources meta files ---------
	LoadMetaFiles();

	//Check the user assets ---------------------
	LOG("Assets Updated with %i changes!", CheckAssetsResources());

	return true;
}

update_status ResourcesManager::Update(float dt)
{
	if (metas_timer.Read() > METAS_UPDATE_RATE)
	{
		metas_timer.Start();
		UpdateMetaFiles();
	}

	App->imgui->GetWorkspace()->BeginDock("Resource Inspector", NULL, NULL);
		
	uint size = inspected_resources.size();
	for (uint k = 0; k < size; k++)
	{
		inspected_resources[k]->BlitComplexUI();
	}

	App->imgui->GetWorkspace()->EndDock();


	return UPDATE_CONTINUE;
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

Resource * ResourcesManager::CreateResource(RESOURCE_TYPE type, uint id)
{
	Resource* new_res = nullptr;

	//Generate the new resource
	switch (type)
	{
	case MESH_RESOURCE:			new_res = (Resource*)new ResourceMesh();				break;
	case MATERIAL_RESOURCE:		new_res = (Resource*)new ResourceMaterial();			break;
	case SCENE_RESOURCE:		new_res = new Resource(RESOURCE_TYPE::SCENE_RESOURCE);	break;
	case SCRIPT_RESOURCE:		new_res = (Resource*)new ResourceScript();				break;
	}
	
	//Add it to the map
	if (new_res != nullptr)
	{
		if (id != 0)new_res->SetID(id);
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

std::vector<Resource*>* ResourcesManager::GetResourcesVectorByType(RESOURCE_TYPE type)
{
	temp_res_vector.clear();

	std::map<uint, Resource*>::const_iterator item = resources.begin();
	while (item != resources.end())
	{
		if (!item._Ptr->_Myval.second->GetConstInMemory() && item._Ptr->_Myval.second->GetResourceType() == type)
		{
			temp_res_vector.push_back(item._Ptr->_Myval.second);
		}

		item++;
	}

	return &temp_res_vector;
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

Resource * ResourcesManager::Find(const char * file_path, RESOURCE_TYPE type) const
{
	for (map<uint, Resource*>::const_iterator res = resources.begin(); res != resources.end(); res++)
	{
		if (strcmp(res->second->GetOriginalFile(), file_path) == 0 && res->second->GetResourceType() == type)return res._Ptr->_Myval.second;
	}
	return nullptr;
}

Resource * ResourcesManager::Find(uint id) const
{
	for (map<uint, Resource*>::const_iterator res = resources.begin(); res != resources.end(); res++)
	{
		if (res->first == id)return res._Ptr->_Myval.second;
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

void ResourcesManager::FindRelatedResources(const char * path, list<Resource*>& _resources)
{
	for (map<uint, Resource*>::const_iterator res = resources.begin(); res != resources.end(); res++)
	{
		if (res->second->GetConstInMemory())continue;

		if (strcmp(res->second->GetOriginalFile(),path) == 0)
		{
			_resources.push_back(res->second);
		}
	}
}

void ResourcesManager::SetInspectedResources(const char * path)
{
	inspected_resources.clear();

	for (map<uint, Resource*>::const_iterator res = resources.begin(); res != resources.end(); res++)
	{
		if (res->second->GetConstInMemory())continue;

		if (strcmp(res->second->GetOriginalFile(), path) == 0)
		{
			inspected_resources.push_back(res->second);
		}
	}
}

void ResourcesManager::LoadMetaFiles()
{
	//Set String to look inside Parent folder
	char str[250];
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
				new_resource = CreateResource(ty, meta_data.GetInt("id"));
				new_resource->Load(meta_data);
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

void ResourcesManager::UpdateMetaFiles()
{
	for (map<uint, Resource*>::const_iterator res = resources.begin(); res != resources.end(); res++)
	{
		if (!res->second->GetConstInMemory())
		{
			uint last_ed_time = res->second->GetLastEditionTime();
			last_time = last_ed_time;

			//Modification time
			path = res->second->GetOriginalFile();
			App->fs->GetFileNameFromPath(res->second->GetOriginalFile() , &usable_string_a);
			if (App->fs->ExistInAssets(usable_string_a.c_str()))
			{
				real_last_time = std::experimental::filesystem::last_write_time(path);
				last_time = decltype(real_last_time)::clock::to_time_t(real_last_time);

				if (last_time != res->second->GetLastEditionTime())
				{
					res->second->SetLastEditionTime(last_time);
					/*Here re import the resource*/
					res->second->ReImport();
				}
			}
			else
			{
				/*Release the resource and the related lib files*/
				/*std::list<Resource*> res_list;
				FindRelatedResources(res->second->GetOriginalFile(), res_list);
				std::list<Resource*>::iterator item = res_list.begin();
				while(item != res_list.end())
				{
					char str[250];
					sprintf(str, "%s\\%s", App->fs->GetUserRootDir()->GetPath(), item._Ptr->_Myval->GetOwnFile());
					App->fs->RemoveFile(str);

					item++;
				}*/ //Will be implemented in the next assignment
			}

			path.clear();
		}
	}
}

uint ResourcesManager::GetLastEditionTime(const char * _path)
{
	//Modification time
	path = _path;
	real_last_time = std::experimental::filesystem::last_write_time(path);
	last_time = decltype(real_last_time)::clock::to_time_t(real_last_time);

	return last_time;
}

void ResourcesManager::BlitConfigInfo()
{
	for (map<uint, Resource*>::const_iterator res = resources.begin(); res != resources.end(); res++)
	{
		ImGui::Text(res->second->GetOwnFile());
		ImGui::Text("References: %i", res->second->GetReferences());
	}
}

Resource * ResourcesManager::BlitResourceButtonsByType(RESOURCE_TYPE type)
{
	bool blit = false;
	Resource* selected_res = nullptr;

	for (map<uint, Resource*>::const_iterator res = resources.begin(); res != resources.end(); res++)
	{
		if (res->second->GetResourceType() != type)continue;

		App->fs->GetUnformatedFileNameFromPath(res->second->GetOwnFile(), &usable_string_a);
		if (ImGui::Selectable(usable_string_a.c_str()))
		{
			selected_res = (ResourceMesh*)res->second;
			blit = true;
			break;
		}
		blit = true;
		
	}
	
	ImGui::TreePop();

	return selected_res;
}

void ResourcesManager::BlitPrefabsMenu(GameObject* target)
{
	if (ImGui::BeginMenu("Prefabs"))
	{
		for (map<uint, Resource*>::const_iterator res = resources.begin(); res != resources.end(); res++)
		{
			if (res._Ptr->_Myval.second->GetResourceType() == RESOURCE_TYPE::SCENE_RESOURCE)
			{
				App->fs->GetUnformatedFileNameFromPath(res._Ptr->_Myval.second->GetOwnFile(), &usable_string_a);
				
				if (ImGui::Selectable(usable_string_a.c_str()))
				{
					GameObject* root = App->importer->scene_importer.Load(res._Ptr->_Myval.second);
					root->SetParent(target);
				}
			}
		}
		ImGui::EndMenu();
	}
}
