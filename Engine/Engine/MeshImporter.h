#ifndef _MESH_IMPORTER_H_
#define _MESH_IMPORTER_H_

#include "Importer.h"
#include "ComponentMesh.h"
#include"MathGeoLib/Math/float3.h"

class MeshImporter : public Importer
{
public:

	MeshImporter();

public:

	void Import(const char* name, std::vector<math::float3> vertices, std::vector<uint> indices);
	void Load(const char* path);

};
#endif