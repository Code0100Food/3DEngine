#ifndef _MATERIAL_IMPORTER_H_
#define _MATERIAL_IMPORTER_H_

#include "Importer.h"

class ComponentMaterial;
class ResourceMaterial;

class MaterialImporter : public Importer
{
public:
	
	MaterialImporter();

public:

	bool Load(const char* path, ComponentMaterial* target);
	bool Load(ResourceMaterial* to_load);
	uint Import(const char* path);

};
#endif // !_MATERIAL_IMPORTER_H_
