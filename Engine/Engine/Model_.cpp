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
		LOG("[error] ASSIMP: %s", import.GetErrorString());
		return;
	}
	
	directory = path.substr(0, path.find_last_of('/'));

	//Get root node transformation
	this->SetTransformation(scene->mRootNode->mTransformation);
	this->name = scene->mRootNode->mName.C_Str();
	LOG("Loading %s model!", this->name.c_str());

	LOG("Loading %i meshes...", scene->mNumMeshes);
	
	ProcessNode(scene->mRootNode, scene);
}

void Model_::ProcessNode(aiNode * node, const aiScene * scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh_ n_mesh = ProcessMesh(mesh, scene);
		
		n_mesh.SetTransformation(node->mTransformation);
		n_mesh.name = node->mName.C_Str();

		meshes.push_back(n_mesh);
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

		LOG("- Material with index %i loaded!", mesh->mMaterialIndex);
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
			texture.id = App->textures->LoadTexture(str.C_Str(), "Assets/");
			texture.type = typeName;
			texture.path = str.C_Str();
			
			//Set texture width and height
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texture.tex_width);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texture.tex_height);
				
			n_textures.push_back(texture);
			
			textures.push_back(texture); // add to loaded textures
		}
	}

	return n_textures;
}

const char * Model_::GetName() const
{
	return name.c_str();
}

void Model_::SetTransformation(aiMatrix4x4 mat)
{
	transformation = mat;
	mat.Decompose(scale, rotation, position);
}

void Model_::BlitInfo() const
{
	//Header of the model
	if (ImGui::CollapsingHeader(("%s", name.c_str()), NULL))
	{
		//Show model position
		ImGui::Text("Position	");
		ImGui::SameLine();
		ImGui::Text("X %.1f		", position.x);
		ImGui::SameLine();
		ImGui::Text("Y %.1f		", position.y);
		ImGui::SameLine();
		ImGui::Text("Z %.1f", position.z);

		float eX = atan2(-2 * (rotation.y*rotation.z - rotation.w*rotation.x), rotation.w*rotation.w - rotation.x*rotation.x - rotation.y*rotation.y + rotation.z*rotation.z);
		float eY = asin(2 * (rotation.x*rotation.z + rotation.w*rotation.y));
		float eZ = atan2(-2 * (rotation.x*rotation.y - rotation.w*rotation.z), rotation.w*rotation.w + rotation.x*rotation.x - rotation.y*rotation.y - rotation.z*rotation.z);

		//Show model rotation
		ImGui::Text("Rotation	");
		ImGui::SameLine();
		ImGui::Text("X %.1f		", eX);
		ImGui::SameLine();
		ImGui::Text("Y %.1f		", eY);
		ImGui::SameLine();
		ImGui::Text("Z %.1f", eZ);

		//Show model scale
		ImGui::Text("Scale	");
		ImGui::SameLine();
		ImGui::Text("X %.1f		", scale.x);
		ImGui::SameLine();
		ImGui::Text("Y %.1f		", scale.y);
		ImGui::SameLine();
		ImGui::Text("Z %.1f", scale.z);

		

		//Iterate all the meshes to blit the info
		uint size = meshes.size();
		for (uint k = 0; k < size; k++)
		{
			meshes[k].BlitInfo();
		}
	}
}
