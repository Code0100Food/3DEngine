#include "ImporterManager.h"

#include "Application.h"
#include "FileSystem.h"

#include "MaterialImporter.h"
#include "ModelImporter.h"
#include "MeshImporter.h"

// Constructors =================================
ImporterManager::ImporterManager()
{

}

// Functionality ================================
bool ImporterManager::ImportFile(const char * str)
{
	bool ret = false;
	std::string format;
	App->fs->GetFileFormatFromPath(str, &format);
	IMPORT_TYPE type = GetImportTypeFromFormat(format.c_str());

	switch (type)
	{
	case UNDEF_IMPORT:
	{
		LOG("[error] File extension not supported for import!");
	}
	break;

	case MESH_IMPORT:
	{
		ret = true; //Because we need to load the file but its already imported
	}
	break;

	case MATERIAL_IMPORT:
	{
		//ret = material_importer.Import(str);
	}
	break;

	case MODEL_IMPORT:
	{
		ret = model_importer.Import(str);
	}
	break;

	default:
	{
		LOG("[error] File extension not supported for import!");
	}
	break;
	}

	return ret;
}

bool ImporterManager::LoadFile(const char * str)
{
	bool ret = false;
	std::string format;
	App->fs->GetFileFormatFromPath(str, &format);
	IMPORT_TYPE type = GetImportTypeFromFormat(format.c_str());
	switch (type)
	{
	case UNDEF_IMPORT:
	{ 
		LOG("[error] File extension not supported for import!");
	}
	break;

	case MESH_IMPORT:
	{
		ret = mesh_importer.Load(str);
	}
	break;

	case MODEL_IMPORT:
	{
		ret = model_importer.Load(str);
	}
	break;
	case MATERIAL_IMPORT:
	{
		
	}
	break;
	default:
	{
		LOG("[error] File extension not supported for import!");
	}
	break;
	}

	return ret;
}

IMPORT_TYPE ImporterManager::GetImportTypeFromFormat(const char * str) const
{
	if (strcmp(str, "fbx") == 0)return MODEL_IMPORT;
	if (strcmp(str, "FBX") == 0)return MODEL_IMPORT;
	if (strcmp(str, "obj") == 0)return MODEL_IMPORT;
	if (strcmp(str, "OBJ") == 0)return MODEL_IMPORT;
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
