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
	file_path = path;

	//Get the model path
	App->fs->GetFolderFromPath(path, &cur_path);

	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		LOG("[error] ASSIMP: %s", import.GetErrorString());
		return false;
	}

	LOG("Loading %s model!", scene->mRootNode->mName.C_Str());

	//Load all the meshes & materials
	LOG("Loading %i meshes...", scene->mNumMeshes);
	ImportNode(scene->mRootNode, scene);

	return true;
}

void SceneImporter::ImportNode(aiNode * node, const aiScene * scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		//Import the mesh and the related materials
		ImportMesh(node->mName.C_Str(), scene->mMeshes[node->mMeshes[i]], scene);
	}

	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ImportNode(node->mChildren[i], scene);
	}
}

void SceneImporter::ImportMesh(const char * name, aiMesh * mesh, const aiScene * scene)
{
	//Generate the container mesh component
	Serializer meta_file;
	ResourceMesh* resource_mesh = (ResourceMesh*)App->res_manager->CreateResource(RESOURCE_TYPE::MESH_RESOURCE);
	
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
		//Generate the container material component (used by mesh too)
		
		//Get the material
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		//Load Normal data
		ImportMaterialTextures(material, aiTextureType_HEIGHT);
		
		//Load diffuse data
		ImportMaterialTextures(material, aiTextureType_DIFFUSE);
		
		//Load Specular data
		ImportMaterialTextures(material, aiTextureType_SPECULAR);
		
		LOG("- Material with index %i loaded!", mesh->mMaterialIndex);
	}

	//Import the mesh (own format)
	App->importer->mesh_importer.Import(name, indices, vertices);

	//Generate the mesh resource
	resource_mesh->SetOriginalFile(file_path.c_str());
	char f_name[100];
	sprintf(f_name, "%s.fiesta", name);
	resource_mesh->SetOwnFile(f_name);
		
	//Save the the meta file
	resource_mesh->Save(meta_file);
	char* buffer = nullptr;
	uint size = meta_file.Save(&buffer);
	char meta_name[200];
	sprintf(meta_name, "%s.fiesta.meta", name);
	sprintf(meta_name, "%s.meta", resource_mesh->GetOwnFile());
	App->fs->SaveFile(meta_name, buffer, size - 1, LIBRARY_META_FOLDER);

	RELEASE_ARRAY(buffer);

	//Clear the used containers
	vertices.clear();
	indices.clear();
	textures.clear();
	vertices_pos.clear();
}

void SceneImporter::ImportMaterialTextures(aiMaterial * mat, aiTextureType type)
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
		}
	}

	return;
}
