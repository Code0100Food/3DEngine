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

public:

	void Draw();

private:

	std::vector<Texture> textures;
	std::vector<Mesh_>	meshes;
	std::string			directory;

private:

	void					LoadModel(std::string path);
	void					ProcessNode(aiNode *node, const aiScene *scene);
	Mesh_					ProcessMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture>	LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	uint					TextureFromFile(const char *path, const std::string &directory, bool gamma);
};
#endif // !_MODEL_H_
