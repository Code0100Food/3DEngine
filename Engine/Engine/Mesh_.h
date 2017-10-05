#ifndef _MESH_H_
#define _MESH_H_

#include "Primitive_.h"
#include "Globals.h"
#include "MathGeoLib/MathGeoLib.h"

struct Vertex
{
	math::float3 position = {0,0,0};
	math::float3 normals = {0,0,0};
	math::float2 tex_coords = {0,0};
};

struct Texture
{
	uint id = 0;
	std::string type;
	std::string path;
};

class Mesh_
{
public:

	Mesh_(std::vector<Vertex> vertices, std::vector<uint> indices, std::vector<Texture> textures);
	~Mesh_();

public:

	void SetupMesh();
	void Draw();

public:

	std::vector<Vertex>			vertices;
	std::vector<uint>			indices;
	std::vector<Texture>		textures;
	
private:

	uint VertexArrayObject = 0, VertexBufferObject = 0, ElementBufferObject = 0;
	uint vectorsID = 0;
	uint normalsID = 0;
	uint text_coordsID = 0;

};
#endif // !_MESH_H_
