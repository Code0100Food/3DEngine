#ifndef _MODULE_IMPORTER_H_
#define _MODULE_IMPORTER_H_

#include "Module.h"
#include "Importer.h"

#include "MeshImporter.h"
#include "MaterialImporter.h"
#include "SceneImporter.h"

class ImporterManager
{
public:

	ImporterManager();

public:

	MeshImporter		mesh_importer;
	MaterialImporter	material_importer;
	SceneImporter		scene_importer;

public:

	bool ImportFile(const char* str);
	bool LoadFile(const char* str);

	IMPORT_TYPE	GetImportTypeFromFormat(const char* str)const;

};
#endif // !_MODULE_IMPORTER_H_
