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

	bool Import(const char* name, std::vector<uint> indices, std::vector<Vertex> vertices, std::vector<Texture> textures);
	bool Load(const char* path);
	bool Load(const char* path, ComponentMesh* target);

};
#endif