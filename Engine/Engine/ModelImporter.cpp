#include "ModelImporter.h"
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

// Constructors =================================
ModelImporter::ModelImporter(const char * path)
{
	LoadModel(path);
}

// Functionality ================================
void ModelImporter::LoadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		LOG("[error] ASSIMP: %s", import.GetErrorString());
		return;
	}
	
	LOG("Loading %s model!", scene->mRootNode->mName.C_Str());

	//Generate root game object
	GameObject* obj = App->scene->CreateGameObject();
	obj->SetName(scene->mRootNode->mName.C_Str());
	
	//Set root transformation
	ComponentTransform* comp = (ComponentTransform*)obj->CreateComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
	comp->SetTransformation(scene->mRootNode->mTransformation);

	//Load all the meshes & materials
	LOG("Loading %i meshes...", scene->mNumMeshes);
	ProcessNode(scene->mRootNode, scene, obj);
	
	//Focus the loaded game object
	App->scene->SetSelectedGameObject(obj);

	//GenerateBoundingBox();
}

void ModelImporter::ProcessNode(aiNode * node, const aiScene * scene, GameObject* parent)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		//Generate game object
		GameObject* obj = App->scene->CreateGameObject();
		obj->SetName(node->mName.C_Str());
		obj->SetParent(parent);
		
		//Set transformation component
		ComponentTransform* comp = (ComponentTransform*)obj->CreateComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
		comp->SetTransformation(node->mTransformation);

		//Generate mesh obj
		ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene, obj);
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, parent);
	}
}

void ModelImporter::ProcessMesh(aiMesh * mesh, const aiScene * scene, GameObject* container)
{
	//Generate the container mesh component
	ComponentMesh* comp_mesh = (ComponentMesh*)container->CreateComponent(COMPONENT_TYPE::COMP_MESH);

	std::vector<Vertex>		vertices;
	std::vector<uint>		indices;
	std::vector<Texture>	textures;

	LOG("Processing %s mesh!", mesh->mName.C_Str());

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
	//Set mesh component vertices
	comp_mesh->SetVertices(vertices);

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
	//Set mesh component indices
	comp_mesh->SetIndices(indices);

	LOG("- %i Faces loaded!", mesh->mNumFaces);

	//Build the different materials (textures)
	if (mesh->mMaterialIndex >= 0)
	{
		//Generate the container material component (used by mesh too)
		ComponentMaterial* comp_material = (ComponentMaterial*)container->CreateComponent(COMPONENT_TYPE::COMP_MATERIAL);

		//Get the material
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		//Load Normal data
		std::vector<Texture> normal_map = LoadMaterialTextures(material,aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normal_map.begin(), normal_map.end());
		//Load diffuse data
		std::vector<Texture> diffuse_map = LoadMaterialTextures(material,aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuse_map.begin(), diffuse_map.end());
		//Load Specular data
		std::vector<Texture> specular_map = LoadMaterialTextures(material,aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specular_map.begin(), specular_map.end());

		//Set material component textures
		comp_material->SetTextures(textures);
		//Set mesh material
		comp_mesh->SetDrawMaterial(comp_material);

		LOG("- Material with index %i loaded!", mesh->mMaterialIndex);
	}

	//Initialize the mesh buffers
	comp_mesh->SetupMesh();
}

std::vector<Texture> ModelImporter::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> n_textures;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textures.size(); j++)
		{
			if (std::strcmp(textures[j].path.c_str(), str.C_Str()) == 0)
			{
				n_textures.push_back(textures[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			// if texture hasn't been loaded already, load it
			Texture texture;
			texture.id = App->textures->LoadTexture(str.C_Str(), "Assets/");
			texture.type = typeName;
			texture.path = str.C_Str();
			
			//Set texture width and height
			glBindTexture(GL_TEXTURE_2D, texture.id);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texture.tex_width);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texture.tex_height);
			glBindTexture(GL_TEXTURE_2D, 0);

			n_textures.push_back(texture);
			
			textures.push_back(texture); // add to loaded textures
		}
	}

	return n_textures;
}