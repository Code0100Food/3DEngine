#ifndef _MATERIAL_IMPORTER_H_
#define _MATERIAL_IMPORTER_H_

#include "Importer.h"

class MaterialImporter : public Importer
{
public:
	
	MaterialImporter();

public:

	void Import(const char* path);

};
#endif // !_MATERIAL_IMPORTER_H_
