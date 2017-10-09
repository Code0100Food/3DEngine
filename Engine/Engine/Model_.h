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

	std::vector<Texture>	textures;
	std::vector<Mesh_>		meshes;
	std::string				directory;
	
	aiMatrix4x4				transformation;
	aiVector3D				position;
	aiQuaternion			rotation;
	aiVector3D				scale;

private:

	void					LoadModel(std::string path);
	void					ProcessNode(aiNode *node, const aiScene *scene);
	Mesh_					ProcessMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture>	LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

public:

	const char*	GetName()const;

	void	SetTransformation(aiMatrix4x4 mat);
	void	BlitInfo()const;

};
#endif // !_MODEL_H_
