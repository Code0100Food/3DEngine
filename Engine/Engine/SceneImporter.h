#ifndef _SCENE_IMPORTER_H_
#define _SCENE_IMPORTER_H_

class	aiNode;
class	aiMesh;
struct	aiScene;
enum	aiTextureType;
class	aiMaterial;
class	GameObject;
class	Serializer;
class	ResourceMesh;
class	Resource;

#include <map>
#include <list>
#include "ComponentMesh.h"
#include "Assimp/include/matrix4x4.h"
#include "Assimp/include/types.h"

class SceneImporter
{
public:

	SceneImporter();

private:

	std::string cur_path;
	std::string file_path;
	
	std::vector<Vertex>			vertices;
	std::vector<uint>			indices;
	std::vector<math::float3>	vertices_pos;

	std::vector<std::string>	textures;

	std::map<uint, aiMesh*>		loaded_meshes;
	std::map<uint, aiMaterial*>	loaded_materials;

	std::list<Resource*>		reimport_targets;

	mutable std::string usable_str_a;
	mutable std::string usable_str_b;

	mutable aiMatrix4x4 inherited_trans;
	mutable aiString	material_str;
public:

	uint Import(const char* path);
	bool ReImport(const char* path);
	bool Load(Resource* target);

private:
	
	void ImportNode(aiNode *node, const aiScene *scene, GameObject* parent);
	void ImportMesh(const char* name, aiMesh *mesh, const aiScene *scene, GameObject* container);
	void ImportMaterialTextures(aiMaterial *mat, aiTextureType type, ComponentMaterial* container);
	
	void ReImportNode(aiNode *node, const aiScene *scene, GameObject* parent);
	void ReImportMesh(const char* name, aiMesh *mesh, const aiScene *scene, GameObject* container);
	void ReImportMaterialTextures(aiMaterial *mat, aiTextureType type, ComponentMaterial* container);

	const char* SceneImporter::AiTextureTypeToStr(aiTextureType ty);
	aiTextureType SceneImporter::StrToAiTextureType(const char* str);
};
#endif // !_MODEL_IMPORTER_H_
