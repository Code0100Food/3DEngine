#ifndef _COMPONENT_MESH_H_
#define _COMPONENT_MESH_H_

#include "Component.h"
#include "MathGeoLib/MathGeoLib.h"

class ComponentMeshRenderer;

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

// Vertex ---------------------------------------
struct Vertex
{
	Vertex();
	Vertex(math::float3 position, math::float3 normals, math::float2 tex_coords);
	
	math::float3 position = { 0,0,0 };
	math::float3 normals = { 0,0,0 };
	math::float2 tex_coords = { 0,0 };
};
// ----------------------------------------------

// Texture --------------------------------------
struct Texture
{
	uint		id = 0;
	std::string type;
	std::string path;

	int			tex_width = 0;
	int			tex_height = 0;

	//Methods ---------------
	void BlitUI()const;
};
// ----------------------------------------------

class ComponentMaterial;

class ComponentMesh : public Component
{

	friend class ComponentMeshRenderer;

public:

	ComponentMesh();
	ComponentMesh(const ComponentMesh& cpy);
	~ComponentMesh();

private:

	std::vector<Vertex>			vertices;
	std::vector<uint>			indices;
	std::vector<math::float3>	bounding_box;

	uint						num_tris = 0;
	uint						num_vertex = 0;

	ComponentMaterial*			draw_material = nullptr;

private:

	uint VertexArrayObject = 0, VertexBufferObject = 0, ElementBufferObject = 0;
	
	uint face_normalsID = 0;
	uint vertex_normalsID = 0;
	uint text_coordsID = 0;

public:

	//Get Methods -----------
	ComponentMaterial* GetDrawMaterial()const;

	//Set Methods -----------
	void	SetVertices(std::vector<Vertex> v);
	void	SetIndices(std::vector<uint> i);
	void	SetDrawMaterial(ComponentMaterial* mat);

	//Functionality ---------
	void	SetupMesh();
	void	GenerateBoundingBox();
	void	DeleteBuffers();
	void	BlitComponentInspector();
};
#endif // !_COMPONENT_MESH_H_
