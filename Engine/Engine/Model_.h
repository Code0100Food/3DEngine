#ifndef _MODEL_H_
#define _MODEL_H_

#include "Mesh_.h"

class aiNode;
class aiMesh;
struct aiScene;
enum aiTextureType;
class aiMaterial;
class GameObject;

class Model_
{
public:

	Model_(const char* path);
	~Model_();

public:

	void Draw();

private:

	std::string				name;
	RENDER_FLAGS			render_flags = REND_NONE;

	std::vector<Texture>	textures;
	std::vector<Mesh_>		meshes;
	std::string				directory;
	
	aiMatrix4x4				transformation;
	aiVector3D				position;
	aiQuaternion			rotation;
	aiVector3D				scale;

	uint					num_tris = 0;
	uint					num_vertex = 0;

	std::vector<math::float3> bounding_box;

private:

	void					LoadModel(std::string path);
	void					ProcessNode(aiNode *node, const aiScene *scene, GameObject* parent);
	Mesh_					ProcessMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture>	LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

	void DrawBoundingBox()const;

public:

	//Get Methods -----------
	const char*		GetName()const;
	math::float3	GetPosition()const;
	float			GetFocusDistance()const;

	//Set Methods -----------
	void		SetTransformation(aiMatrix4x4 mat);
	void		SetRenderFlags(RENDER_FLAGS n_flag);

	//Functionality ---------
	void		GenerateBoundingBox();
	void		BlitInfo();

};
#endif // !_MODEL_H_
