#ifndef _MATERIAL_IMPORTER_H_
#define _MATERIAL_IMPORTER_H_

#include "Importer.h"

class MaterialImporter : public Importer
{
public:
	
	MaterialImporter(const char* path, const void * buffer, unsigned int size);

private:


};
#endif // !_MATERIAL_IMPORTER_H_
