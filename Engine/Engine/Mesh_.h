#ifndef _MESH_H_
#define _MESH_H_

#include "Primitive_.h"
#include "Globals.h"
#include "MathGeoLib/MathGeoLib.h"
#include "Assimp/include/matrix4x4.h"
#include "Assimp/include/vector3.h"
#include "Assimp/include/quaternion.h"

// Render Flags ---------------------------------
enum RENDER_FLAGS
{
	REND_NONE = 1 << 0,
	REND_VERTEX_NORMALS = 1 << 1,
	REND_FACE_NORMALS = 1 << 2,
	REND_BOUND_BOX = 1 << 3
};

inline RENDER_FLAGS operator~ (RENDER_FLAGS a)
{
	return (RENDER_FLAGS)~(int)a; 
}

inline RENDER_FLAGS& operator&= (RENDER_FLAGS& a, RENDER_FLAGS b)
{
	return (RENDER_FLAGS&)((int&)a &= (int)b);
}

inline RENDER_FLAGS& operator|= (RENDER_FLAGS& a, RENDER_FLAGS b)
{
	return (RENDER_FLAGS&)((int&)a |= (int)b);
}
// ----------------------------------------------

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

	void BlitUI();
};

class Mesh_
{
public:

	Mesh_(std::vector<Vertex> vertices, std::vector<uint> indices, std::vector<Texture> textures);
	~Mesh_();

public:

	void SetupMesh();
	void Draw();
	void ReleaseBuffers();

public:

	std::string				name;
	RENDER_FLAGS			render_flags = REND_NONE;

	std::vector<Vertex>		vertices;
	std::vector<uint>		indices;
	std::vector<Texture>	textures;
	
	aiMatrix4x4				transformation;
	aiVector3D				position;
	aiQuaternion			rotation;
	aiVector3D				scale;

	uint					num_tris = 0;
	uint					num_vertex = 0;

	std::vector<math::float3> bounding_box;

private:

	uint VertexArrayObject = 0, VertexBufferObject = 0, ElementBufferObject = 0;
	uint face_normalsID = 0;
	uint vertex_normalsID = 0;
	uint text_coordsID = 0;

private:

	void DrawVertexNormals()const;
	void DrawFaceNormals()const;
	void DrawBoundingBox()const;
	
public:

	//Get Methods -----------
	const char*	GetName()const;

	//Set Methods -----------
	void		SetTransformation(aiMatrix4x4 mat);
	void		SetRenderFlags(RENDER_FLAGS n_flag);

	//Functionality ---------
	void		GenerateBoundingBox();
	void		BlitInfo(uint index);

};
#endif // !_MESH_H_
