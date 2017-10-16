#ifndef _MATERIAL_IMPORTER_H_
#define _MATERIAL_IMPORTER_H_

#include "Importer.h"

class MaterialImporter : public Importer
{
public:
	
	MaterialImporter();

public:

	bool Load(const char* path);
	bool Import(const char* path);

};
#endif // !_MATERIAL_IMPORTER_H_
