#ifndef _MATERIAL_IMPORTER_H_
#define _MATERIAL_IMPORTER_H_

#include "Importer.h"
#include <string>

class ComponentMaterial;
class ResourceMaterial;

class MaterialImporter : public Importer
{
public:
	
	MaterialImporter();

private:

	mutable std::string usable_str;

public:

	bool Load(const char* path, ComponentMaterial* target);
	bool Load(ResourceMaterial* to_load);
	uint Import(const char* path);
	bool ReImport(ResourceMaterial* to_reload);

};
#endif // !_MATERIAL_IMPORTER_H_
