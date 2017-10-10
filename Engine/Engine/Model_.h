#ifndef _MODEL_H_
#define _MODEL_H_

#include "Mesh_.h"

class aiNode;
class aiMesh;
struct aiScene;
enum aiTextureType;
class aiMaterial;



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

	std::vector<math::float3> bounding_box;

private:

	void					LoadModel(std::string path);
	void					ProcessNode(aiNode *node, const aiScene *scene);
	Mesh_					ProcessMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture>	LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

	void DrawBoundingBox()const;

public:

	//Get Methods -----------
	const char*	GetName()const;

	//Set Methods -----------
	void		SetTransformation(aiMatrix4x4 mat);
	void		SetRenderFlags(RENDER_FLAGS n_flag);

	//Functionality ---------
	void		GenerateBoundingBox();
	void		BlitInfo();

};
#endif // !_MODEL_H_
