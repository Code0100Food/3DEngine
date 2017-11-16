#include "ImporterManager.h"

#include "Application.h"
#include "FileSystem.h"
#include "ResourcesManager.h"
#include "ModuleScene.h"

// Constructors =================================
ImporterManager::ImporterManager()
{

}

// Functionality ================================
bool ImporterManager::ImportFile(const char * path, bool place_on_scene)
{
	if (App->res_manager->Find(path) != nullptr)return true; //If is already imported!

	bool b_ret = false;

	//Get the file format to call the correct importer
	std::string format;
	App->fs->GetFileFormatFromPath(path, &format);
	IMPORT_TYPE imp_type = App->importer->GetImportTypeFromFormat(format.c_str());

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
		b_ret = material_importer.Import(n_path.c_str());
		if(b_ret && place_on_scene)
		{
			ResourceMaterial* res = (ResourceMaterial*)App->res_manager->FindLast(RESOURCE_TYPE::MATERIAL_RESOURCE);
			//Set the target texture
			GameObject* obj = App->scene->GetSelectedGameObject();
			if (obj != nullptr)
			{
				obj->SetMaterial(res);
			}
		}
		break;
	case MESH_IMPORT:
		break;
	case SCENE_IMPORT:
		b_ret = scene_importer.Import(n_path.c_str());
		if (b_ret && place_on_scene)
		{
			Resource* res = (Resource*)App->res_manager->FindLast(RESOURCE_TYPE::SCENE_RESOURCE);
			//Place the imported scene in the 3D world
			App->importer->scene_importer.Load(res);
		}
		break;
	}

	return b_ret;
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
