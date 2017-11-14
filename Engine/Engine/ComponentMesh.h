#ifndef _COMPONENT_MESH_H_
#define _COMPONENT_MESH_H_

#include "Component.h"
#include "MathGeoLib/MathGeoLib.h"

class ComponentMeshRenderer;
class ResourceMesh;

// Render Flags ---------------------------------
enum RENDER_FLAGS
{
	REND_NONE = 1 << 0,
	REND_VERTEX_NORMALS = 1 << 1,
	REND_FACE_NORMALS = 1 << 2
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

	bool operator == (const Vertex& target)
	{
		return position == target.position && normals == target.normals && tex_coords == target.tex_coords;
	}
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

protected:

	ResourceMesh*		resource_mesh = nullptr;
	ComponentMaterial*	draw_material = nullptr;

public:

	//Get Methods -----------
	ComponentMaterial*			GetDrawMaterial()const;
	std::vector<math::float3>	GetVertexPositions()const;
	uint						GetIndexSize()const;
	int							GetIndexAt(int position) const;
	math::float3				GetVertexPosAt(int position) const;
	uint						GetNumTris()const;
	uint						GetNumVertex()const;
	uint						GetVertexArrayObject()const;
	uint						GetVertexBufferObject()const;
	uint						GetElementBufferObject()const;
	uint						GetFaceNormalsID()const;
	uint						GetVertexNormalsID()const;
	uint						GetTextureCoordsID()const;
	
	//Set Methods -----------
	void	SetVertices(const std::vector<Vertex>& v);
	void	SetIndices(const std::vector<uint>& i);
	void	SetPath(const char* str);
	void	SetDrawMaterial(ComponentMaterial* mat);
	void	SetResourceMesh(ResourceMesh* res);

	//Functionality ---------
	void	SetupMesh();
	bool	MeshResourceIsNull()const;
	void	BlitComponentInspector();

	//Save/Load Methods -----
	bool	Save(Serializer& array_root)const;
	bool	Load(Serializer& data, std::vector<std::pair<Component*, uint>>& links);
	void	LinkComponent(const Component* target);
	void	UnLinkComponent();
};
#endif // !_COMPONENT_MESH_H_
