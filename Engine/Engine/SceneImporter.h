#ifndef _SCENE_IMPORTER_H_
#define _SCENE_IMPORTER_H_

class	aiNode;
class	aiMesh;
struct	aiScene;
enum	aiTextureType;
class	aiMaterial;
class	GameObject;
class	Serializer;
class	Resource;
class	ResourceMaterial;
class	ResourceMesh;

#include <map>
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

	std::map<aiMesh*, ResourceMesh*>			loaded_meshes;
	std::map<aiMaterial*, ResourceMaterial*>	loaded_materials;

	mutable std::string usable_str_a;
	mutable std::string usable_str_b;

public:

	bool Import(const char* path);
	bool Load(Resource* target);

private:
	
	void ImportNode(aiNode *node, const aiScene *scene, GameObject* parent);
	void ImportMesh(const char* name, aiMesh *mesh, const aiScene *scene, GameObject* container);
	void ImportMaterialTextures(aiMaterial *mat, aiTextureType type, ComponentMaterial* container);

};
#endif // !_MODEL_IMPORTER_H_
