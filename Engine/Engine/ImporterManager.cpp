#include "ImporterManager.h"

#include "Application.h"
#include "FileSystem.h"
#include "ResourcesManager.h"
#include "ModuleScene.h"
#include "ModuleTextures.h"

// Constructors =================================
ImporterManager::ImporterManager()
{

}

// Functionality ================================
bool ImporterManager::ImportFile(const char * path, bool place_on_scene)
{
	bool b_ret = false;
	
	//Get the file format to call the correct importer
	Resource* res = nullptr;
	App->fs->GetFileFormatFromPath(path, &format);
	IMPORT_TYPE imp_type = App->importer->GetImportTypeFromFormat(format.c_str());

	switch (imp_type)
	{
	case MATERIAL_IMPORT:
		res = App->res_manager->Find(path, RESOURCE_TYPE::MATERIAL_RESOURCE);
		break;
	case MESH_IMPORT:
		res = App->res_manager->Find(path, RESOURCE_TYPE::MESH_RESOURCE);
		break;
	case SCENE_IMPORT:
		res = App->res_manager->Find(path, RESOURCE_TYPE::SCENE_RESOURCE);
		break;
	}
	//Already imported resource case
	uint res_id = 0;
	if (res == nullptr)
	{
		//Check if the file is in assets
		n_path = path;
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
			res_id = material_importer.Import(n_path.c_str());
			b_ret = res_id != 0;
			break;
		case MESH_IMPORT:
			break;
		case SCENE_IMPORT:
			res_id = scene_importer.Import(n_path.c_str());
			b_ret = res_id != 0;
			break;
		}
	}
	else
	{
		b_ret = true;
	}

	if (place_on_scene)
	{
		switch (imp_type)
		{
		case UNDEF_IMPORT:
			break;
		case MATERIAL_IMPORT:
			if (b_ret && place_on_scene)
			{
				if(res == nullptr)res = App->res_manager->Find(res_id);
				//Set the target texture
				GameObject* obj = App->scene->GetSelectedGameObject();
				if (obj != nullptr)
				{
					obj->SetMaterial((ResourceMaterial*)res);
				}
				//Set textures custom texture
				App->textures->SetCustomTexutreID(((ResourceMaterial*)res)->GetMaterialID());
			}
			break;
		case MESH_IMPORT:
			break;
		case SCENE_IMPORT:
			if (b_ret && place_on_scene)
			{
				if (res == nullptr)res = App->res_manager->Find(res_id);
				//Place the imported scene in the 3D world
				App->importer->scene_importer.Load(res);
			}
			break;
		}
	}

	format.clear();
	n_path.clear();

	return b_ret;
}

bool ImporterManager::ReImportResource(Resource * target)
{
	bool ret = false;

	switch (target->GetResourceType())
	{
	case MESH_RESOURCE:
		ret = mesh_importer.ReImport((ResourceMesh*)target);
		break;
	case MATERIAL_RESOURCE:
		ret = material_importer.ReImport((ResourceMaterial*)target);
		break;
	case SCENE_RESOURCE:
		ret = scene_importer.ReImport(target);
		break;
	}

	return ret;
}

IMPORT_TYPE ImporterManager::GetImportTypeFromFormat(const char * str) const
{
	if (strcmp(str, "fbx") == 0)return SCENE_IMPORT;
	if (strcmp(str, "FBX") == 0)return SCENE_IMPORT;
	if (strcmp(str, "obj") == 0)return SCENE_IMPORT;
	if (strcmp(str, "OBJ") == 0)return SCENE_IMPORT;
	if (strcmp(str, "png") == 0)return MATERIAL_IMPORT;
	if (strcmp(str, "dds") == 0)return MATERIAL_IMPORT;
	if (strcmp(str, "PNG") == 0)return MATERIAL_IMPORT;
	if (strcmp(str, "jpg") == 0)return MATERIAL_IMPORT;
	if (strcmp(str, "JPG") == 0)return MATERIAL_IMPORT;
	if (strcmp(str, "tga") == 0)return MATERIAL_IMPORT;
	if (strcmp(str, "TGA") == 0)return MATERIAL_IMPORT;
	if (strcmp(str, "fiesta") == 0)return MESH_IMPORT;
	return UNDEF_IMPORT;
}
