#include "Importer.h"
#include "ModelImporter.h"

Importer::Importer()
{

}

Importer::~Importer()
{

}

void Importer::ImportScene(const char * path)
{
	ModelImporter imp(path);
}
