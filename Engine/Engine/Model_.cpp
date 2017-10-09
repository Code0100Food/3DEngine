#include "Model_.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include "stb/stb_image.h"
#include "Assimp/include/Importer.hpp"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Application.h"
#include "ModuleTextures.h"

// Constructors =================================
Model_::Model_(const char * path)
{
	LoadModel(path);
}

// Destructors ==================================
Model_::~Model_()
{

}

// Game Loop ====================================
void Model_::Draw()
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw();
	}
}

// Functionality ================================
void Model_::LoadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		LOG("ERROR_ASSIMP: %s", import.GetErrorString());
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	ProcessNode(scene->mRootNode, scene);
}

void Model_::ProcessNode(aiNode * node, const aiScene * scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh_ Model_::ProcessMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<Vertex>		vertices;
	std::vector<uint>		indices;
	std::vector<Texture>	textures;
	
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

	//Build the different materials (textures)
	if (mesh->mMaterialIndex >= 0)
	{
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
	}

	return Mesh_(vertices, indices, textures);
}

std::vector<Texture> Model_::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> n_textures;
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
			texture.id = TextureFromFile(str.C_Str(), directory, true);
			texture.type = typeName;
			texture.path = str.C_Str();
			n_textures.push_back(texture);
			textures.push_back(texture); // add to loaded textures
		}
	}

	return n_textures;
}

unsigned int Model_::TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = "Assets/" + filename;

	return App->textures->LoadTexture(filename.c_str());

	unsigned int textureID = 0;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
		{
			format = GL_RED;
		}
		else if (nrComponents == 3)
		{
			format = GL_RGB;
		}
		else if (nrComponents == 4)
		{
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}

	else
	{
		LOG("Texture failed to load at path: %s", path);
		stbi_image_free(data);
	}

	return textureID;
}
