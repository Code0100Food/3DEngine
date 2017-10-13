#ifndef _COMP_MESH_RENDERER_H_
#define _COMP_MESH_RENDERER_H_

#include "ComponentMesh.h"

class ComponentMeshRenderer : public Component
{
public:

	ComponentMeshRenderer();
	ComponentMeshRenderer(const ComponentMeshRenderer& cpy);
	~ComponentMeshRenderer();

public:

	bool Update();

private:

	RENDER_FLAGS	render_flags = REND_NONE;
	ComponentMesh*	target_mesh = nullptr;

private:

	void DrawVertexNormals()const;
	void DrawFaceNormals()const;
	void DrawBoundingBox()const;

public:

	//Set Methods -----------
	void SetTargetMesh(const ComponentMesh* target);

	//Functionality ---------
	void BlitComponentInspector();

};
#endif // !_COMP_MESH_RENDERER_H_
