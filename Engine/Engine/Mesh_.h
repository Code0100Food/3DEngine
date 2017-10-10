#ifndef _MESH_H_
#define _MESH_H_

#include "Primitive_.h"
#include "Globals.h"
#include "MathGeoLib/MathGeoLib.h"
#include "Assimp/include/matrix4x4.h"
#include "Assimp/include/vector3.h"
#include "Assimp/include/quaternion.h"

struct Vertex
{
	math::float3 position = {0,0,0};
	math::float3 normals = {0,0,0};
	math::float2 tex_coords = {0,0};
};

struct Texture
{
	uint		id = 0;
	std::string type;
	std::string path;

	int		tex_width = 0;
	int		tex_height = 0;
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

	std::string				name;

	std::vector<Vertex>		vertices;
	std::vector<uint>		indices;
	std::vector<Texture>	textures;
	
	aiMatrix4x4				transformation;
	aiVector3D				position;
	aiQuaternion			rotation;
	aiVector3D				scale;

private:

	uint VertexArrayObject = 0, VertexBufferObject = 0, ElementBufferObject = 0;
	uint face_normalsID = 0;
	uint normalsID = 0;
	uint text_coordsID = 0;

public:

	const char* GetName()const;

	void	SetTransformation(aiMatrix4x4 mat);
	void	BlitInfo()const;

};
#endif // !_MESH_H_
