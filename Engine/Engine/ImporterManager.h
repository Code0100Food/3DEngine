#ifndef _MODULE_IMPORTER_H_
#define _MODULE_IMPORTER_H_

#include "Module.h"
#include "Importer.h"
#include"MathGeoLib/Math/float3.h"

class ImporterManager
{
public:

	ImporterManager();

public:

	bool ImportFile(const char* str);
	bool ImportMesh(const char* name, std::vector<math::float3> vertices, std::vector<uint> indices);

	IMPORT_TYPE	GetImportTypeFromFormat(const char* str)const;

};
#endif // !_MODULE_IMPORTER_H_
