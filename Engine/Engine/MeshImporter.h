#ifndef _MESH_IMPORTER_H_
#define _MESH_IMPORTER_H_

#include "Importer.h"
#include "ComponentMesh.h"
#include"MathGeoLib/Math/float3.h"

class ResourceMesh;

class MeshImporter : public Importer
{
public:

	MeshImporter();

private:

	std::vector<uint> indices;
	std::vector<Vertex> vertices;

	std::vector<math::float3> v_pos;
	std::vector<math::float3> v_norm;
	std::vector<math::float2> v_tex_coords;

public:

	bool Import(const char* name, std::vector<uint> indices, std::vector<Vertex> vertices);
	bool ReImport(ResourceMesh* to_reload);
	bool Load(const char* path);
	bool Load(ResourceMesh* target);
	bool Load(const char* path, ComponentMesh* target);

};
#endif