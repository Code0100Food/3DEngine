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
		LOG("[error] File extension not supported!");
		break;

	case MATERIAL_IMPORT:
	{
		material_importer.Load(str);
	}
	break;

	case MESH_IMPORT:
	{
		
	}
	break;

	case MODEL_IMPORT:
	{
		model_importer.Load(str);
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
	if (strcmp(str, "PNG") == 0)return MATERIAL_IMPORT;
	if (strcmp(str, "jpg") == 0)return MATERIAL_IMPORT;
	if (strcmp(str, "JPG") == 0)return MATERIAL_IMPORT;
	return UNDEF_IMPORT;
}
