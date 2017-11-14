#ifndef _IMPORTER_H_
#define _IMPORTER_H_

#include "Globals.h"

enum IMPORT_TYPE
{
	UNDEF_IMPORT = 0,
	MATERIAL_IMPORT,
	MESH_IMPORT,
	SCENE_IMPORT
};

class Importer
{
public:

	Importer();
	~Importer();

};
#endif // !_IMPORTER_H_
