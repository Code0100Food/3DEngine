#ifndef _MODULE_IMPORTER_H_
#define _MODULE_IMPORTER_H_

#include "Module.h"
#include "Importer.h"

#include "MeshImporter.h"
#include "MaterialImporter.h"
#include "SceneImporter.h"
#include "ScriptImporter.h"

class Resource;

class ImporterManager
{
public:

	ImporterManager();

private:

	std::string format;
	std::string n_path;

public:

	MeshImporter		mesh_importer;
	MaterialImporter	material_importer;
	SceneImporter		scene_importer;
	ScriptImporter		script_importer;

public:

	bool ImportFile(const char* str, bool place_on_scene = false);
	bool ReImportResource(Resource* target);

	IMPORT_TYPE	GetImportTypeFromFormat(const char* str)const;

};
#endif // !_MODULE_IMPORTER_H_
