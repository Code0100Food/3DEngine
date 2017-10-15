#ifndef _MODEL_IMPORTER_H_
#define _MODEL_IMPORTER_H_

class	aiNode;
class	aiMesh;
struct	aiScene;
enum	aiTextureType;
class	aiMaterial;
class	GameObject;

#include "ComponentMesh.h"

class ModelImporter
{
public:

	ModelImporter();

private:

	std::string cur_path;

public:

	bool	Load(const char* path);
	bool	Import(const char* path);

private:
	
	void					ProcessNode(aiNode *node, const aiScene *scene, GameObject* parent);
	void					ProcessMesh(const char* name, aiMesh *mesh, const aiScene *scene, GameObject* container);
	std::vector<Texture>	LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

};
#endif // !_MODEL_IMPORTER_H_
