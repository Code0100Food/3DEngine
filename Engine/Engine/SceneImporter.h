#ifndef _SCENE_IMPORTER_H_
#define _SCENE_IMPORTER_H_

class	aiNode;
class	aiMesh;
struct	aiScene;
enum	aiTextureType;
class	aiMaterial;
class	GameObject;

#include "ComponentMesh.h"

class SceneImporter
{
public:

	SceneImporter();

private:

	std::string cur_path;
	std::string file_path;

	std::vector<Vertex>			vertices;
	std::vector<uint>			indices;
	std::vector<Texture>		textures;
	std::vector<math::float3>	vertices_pos;

	std::vector<Texture> n_textures;
	std::vector<Texture> _textures;

public:

	bool	Import(const char* path);

private:
	
	void					ImportNode(aiNode *node, const aiScene *scene);
	void					ImportMesh(const char* name, aiMesh *mesh, const aiScene *scene);
	void					ImportMaterialTextures(aiMaterial *mat, aiTextureType type);

};
#endif // !_MODEL_IMPORTER_H_
