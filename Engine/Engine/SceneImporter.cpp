#include "SceneImporter.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include "stb/stb_image.h"
#include "Assimp/include/Importer.hpp"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleScene.h"
#include "ImporterManager.h"
#include "FileSystem.h"
#include "ResourcesManager.h"
#include "Serializer.h"

// Constructors =================================
SceneImporter::SceneImporter()
{

}

// Functionality ================================
bool SceneImporter::Import(const char * path)
{
	//Get paths data
	file_path = path;
	App->fs->GetUnformatedFileNameFromPath(path, &usable_str);
	App->fs->GetFolderFromPath(path, &cur_path);

	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		LOG("[error] ASSIMP: %s", import.GetErrorString());
		return false;
	}

	LOG("Loading %s scene!", usable_str.c_str());

	Serializer scene_structure;

	//Load all the meshes & materials
	LOG("Loading %i meshes...", scene->mNumMeshes);

	//Generate root game object
	GameObject* obj = App->scene->CreateGameObject();
	obj->SetName(usable_str.c_str());

	//scene->mRootNode->mTransformation
	ImportNode(scene->mRootNode, scene, scene_structure);

	App->scene->SerializeScene(obj, scene_structure);
	RELEASE(obj);

	//Generate the scene resource & json structure file
	Resource* scene_res = App->res_manager->CreateResource(RESOURCE_TYPE::SCENE_RESOURCE);
	Serializer meta_file;
	scene_res->SetOriginalFile(path);
	char own_name[200];
	sprintf(own_name, "%s.scn", usable_str.c_str());
	scene_res->SetOwnFile(own_name);

	//Save the generated structure file
	char* buffer = nullptr;
	uint size = scene_structure.Save(&buffer);
	App->fs->SaveFile(scene_res->GetOwnFile(), buffer, size - 1, LIBRARY_SCENE_FOLDER);
	RELEASE_ARRAY(buffer);
	
	char meta_name[200];
	sprintf(meta_name, "%s.meta", scene_res->GetOwnFile());
	scene_res->Save(meta_file);
	size = meta_file.Save(&buffer);
	App->fs->SaveFile(meta_name, buffer, size - 1, LIBRARY_META_FOLDER);
	RELEASE_ARRAY(buffer);

	return true;
}

bool SceneImporter::Load(Resource * target)
{
	return true;
}

void SceneImporter::ImportNode(aiNode * node, const aiScene * scene, Serializer& root)
{
	Serializer  node_root = root.InsertChild(node->mName.C_Str());
	
	// process all the node's meshes (if any)
	if (node->mNumMeshes > 0)
	{
		Serializer meshes_array = node_root.InsertArray("meshes");
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			Serializer mesh_struc;
			
			//Import the mesh and the related materials
			ImportMesh(node->mName.C_Str(), scene->mMeshes[node->mMeshes[i]], scene, mesh_struc);

			meshes_array.InsertArrayElement(mesh_struc);
		}
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ImportNode(node->mChildren[i], scene, node_root);
	}
}

void SceneImporter::ImportMesh(const char * name, aiMesh * mesh, const aiScene * scene, Serializer& root)
{
	//Generate the container mesh component
	ResourceMesh* resource_mesh = (ResourceMesh*)App->res_manager->CreateResource(RESOURCE_TYPE::MESH_RESOURCE);
	root.InsertInt("id", resource_mesh->GetID());

	LOG("Processing %s mesh!", name);

	//Iterate all the mesh vertices and load all the data
	for (uint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex			vertex;
		math::float3	data;
		math::float2	tex_color;
		//Build vertex positions
		if (mesh->HasPositions())
		{
			data.x = mesh->mVertices[i].x;
			data.y = mesh->mVertices[i].y;
			data.z = mesh->mVertices[i].z;
			vertex.position = data;
			vertices_pos.push_back(data);
		}

		//Build vertex normals
		if (mesh->HasNormals())
		{
			data.x = mesh->mNormals[i].x;
			data.y = mesh->mNormals[i].y;
			data.z = mesh->mNormals[i].z;
			vertex.normals = data;
		}

		//Build texture coordinates
		if (mesh->HasTextureCoords(0))
		{
			tex_color.x = mesh->mTextureCoords[0][i].x;
			tex_color.y = mesh->mTextureCoords[0][i].y;
			vertex.tex_coords = tex_color;
		}

		//Add the built vertex to the vertex vector
		vertices.push_back(vertex);
	}

	LOG("- %i Vertices loaded!", mesh->mNumVertices);

	//Build the triangles with the index
	if (mesh->HasFaces())
	{
		for (uint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			for (uint j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}
	}

	LOG("- %i Faces loaded!", mesh->mNumFaces);

	//Build the different materials (textures)
	if (mesh->mMaterialIndex >= 0)
	{
		//Generate the materials array
		Serializer materials_array = root.InsertArray("materials");
		
		//Get the material
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		//Load Normal data
		ImportMaterialTextures(material, aiTextureType_HEIGHT, materials_array);
		
		//Load diffuse data
		ImportMaterialTextures(material, aiTextureType_DIFFUSE, materials_array);
		
		//Load Specular data
		ImportMaterialTextures(material, aiTextureType_SPECULAR, materials_array);
		
		LOG("- Material with index %i loaded!", mesh->mMaterialIndex);
	}

	//Import the mesh (own format)
	App->importer->mesh_importer.Import(name, indices, vertices);

	//Generate the mesh resource
	resource_mesh->SetOriginalFile(file_path.c_str());
	char f_name[100];
	sprintf(f_name, "%s.fiesta", name);
	resource_mesh->SetOwnFile(f_name);
		
	//Save the meta file
	Serializer meta_file;
	resource_mesh->Save(meta_file);
	char* buffer = nullptr;
	uint size = meta_file.Save(&buffer);
	char meta_name[200];
	sprintf(meta_name, "%s.meta", resource_mesh->GetOwnFile());
	App->fs->SaveFile(meta_name, buffer, size - 1, LIBRARY_META_FOLDER);

	RELEASE_ARRAY(buffer);

	//Clear the used containers
	vertices.clear();
	indices.clear();
	textures.clear();
	vertices_pos.clear();
}

void SceneImporter::ImportMaterialTextures(aiMaterial * mat, aiTextureType type, Serializer& root)
{
	n_textures.clear();
	_textures.clear();

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < _textures.size(); j++)
		{
			if (std::strcmp(_textures[j].path.c_str(), str.C_Str()) == 0)
			{
				n_textures.push_back(_textures[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			// if texture hasn't been loaded already, load it
			Texture texture;
			texture.path = str.C_Str();

			n_textures.push_back(texture);

			// add to loaded textures
			_textures.push_back(texture);

			//Import material
			std::string file_folder = cur_path;
			file_folder += texture.path.c_str();
			App->importer->ImportFile(file_folder.c_str());
			
			//Find the loaded mat and add the id on the mesh materials
			Resource* mat = App->res_manager->Find(file_folder.c_str());
			if (mat != nullptr)root.InsertArrayInt(mat->GetID());
		}
	}

	return;
}
