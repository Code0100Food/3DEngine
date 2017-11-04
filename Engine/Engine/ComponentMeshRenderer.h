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

	bool Update(float dt);

private:

	RENDER_FLAGS	render_flags = REND_NONE;
	ComponentMesh*	target_mesh = nullptr;

private:

	void DrawVertexNormals()const;
	void DrawFaceNormals()const;

public:

	//Set Methods -----------
	void SetTargetMesh(const ComponentMesh* target);

	//Functionality ---------
	void BlitComponentInspector();

	//Save/Load Methods -----
	bool	Save(Serializer& array_root)const;
	bool	Load(Serializer& data, std::vector<std::pair<Component*, uint>>& links);
	void	LinkComponent(const Component* target);

};
#endif // !_COMP_MESH_RENDERER_H_
