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
uint SceneImporter::Import(const char * path)
{
	//Get paths data
	file_path = path;
	App->fs->GetUnformatedFileNameFromPath(path, &usable_str_a);
	App->fs->GetFolderFromPath(path, &cur_path);

	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		LOG("[error] ASSIMP: %s", import.GetErrorString());
		return 0;
	}

	LOG("Loading %s scene!", usable_str_a.c_str());

	Serializer scene_structure;

	//Load all the meshes & materials
	LOG("Loading %i meshes...", scene->mNumMeshes);

	//Generate load root game object
	GameObject* root_obj = App->scene->CreateGameObject();
	root_obj->CreateComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);

	//Generate real root obj
	GameObject* obj = App->scene->CreateGameObject();
	obj->SetParent(root_obj);
	obj->SetName(usable_str_a.c_str());
	//Set real root transformation
	ComponentTransform* comp = (ComponentTransform*)obj->CreateComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
	comp->SetTransformation(scene->mRootNode->mTransformation);

	//scene->mRootNode->mTransformation
	ImportNode(scene->mRootNode, scene, obj);

	App->scene->SerializeScene(root_obj, scene_structure);
	RELEASE(root_obj);

	//Generate the scene resource & json structure file
	Resource* scene_res = App->res_manager->CreateResource(RESOURCE_TYPE::SCENE_RESOURCE);
	scene_res->SetOriginalFile(file_path.c_str());
	char own_name[200];
	sprintf(own_name, "%s.scn", usable_str_a.c_str());
	scene_res->SetOwnFile(own_name);

	//Save the generated structure file
	char* buffer = nullptr;
	uint size = scene_structure.Save(&buffer);
	App->fs->SaveFile(scene_res->GetOwnFile(), buffer, size - 1, LIBRARY_SCENE_FOLDER);
	RELEASE_ARRAY(buffer);

	scene_res->Save();

	loaded_meshes.clear();
	loaded_materials.clear();

	return scene_res->GetID();
}

bool SceneImporter::Load(Resource * target)
{
	char str[150];
	sprintf(str, "%s%s", LIBRARY_SCENE_FOLDER, target->GetOwnFile());
	return App->scene->LoadSerializedScene(str);
}

void SceneImporter::ImportNode(aiNode * node, const aiScene * scene, GameObject* parent)
{
	GameObject* node_obj = parent;

	if (node->mMetaData != NULL)
	{
		if (node->mNumMeshes == 0)
		{
			//Generate game object
			node_obj = App->scene->CreateGameObject();
			node_obj->SetName(node->mName.C_Str());
			node_obj->SetParent(parent);

			//Set transformation component
			ComponentTransform* node_comp = (ComponentTransform*)node_obj->CreateComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
			node_comp->SetTransformation(node->mTransformation);
		}

		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			//Generate game object
			GameObject* obj = App->scene->CreateGameObject();
			obj->SetName(node->mName.C_Str());
			obj->SetParent(parent);
			
			//Set transformation component
			ComponentTransform* comp = (ComponentTransform*)obj->CreateComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
			comp->SetTransformation(node->mTransformation);

			//Import the mesh and the related materials
			ImportMesh(node->mName.C_Str(), scene->mMeshes[node->mMeshes[i]], scene, obj);
		}
	}

	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ImportNode(node->mChildren[i], scene, node_obj);
	}
}

void SceneImporter::ImportMesh(const char * name, aiMesh * mesh, const aiScene * scene, GameObject* container)
{
	ResourceMesh* resource_mesh = nullptr;
	ComponentMesh* comp_mesh = (ComponentMesh*)container->CreateComponent(COMPONENT_TYPE::COMP_MESH);
	bool found = false;

	std::map<uint, aiMesh*>::const_iterator item = loaded_meshes.begin();
	while (item != loaded_meshes.end())
	{
		if (item._Ptr->_Myval.second == mesh)
		{
			resource_mesh = (ResourceMesh*)App->res_manager->Find(item._Ptr->_Myval.first);
			found = true;
		}

		item++;
	}
	if(!found)resource_mesh = (ResourceMesh*)App->res_manager->CreateResource(RESOURCE_TYPE::MESH_RESOURCE);

	comp_mesh->SetResourceMesh(resource_mesh, false);

	//Generate the container mesh renderer component
	ComponentMeshRenderer* comp_mesh_renderer = (ComponentMeshRenderer*)container->CreateComponent(COMPONENT_TYPE::COMP_MESH_RENDERER);
	comp_mesh_renderer->SetTargetMesh(comp_mesh);

	if (!found)
	{
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
	}

	//Build the different materials (textures)
	if (mesh->mMaterialIndex >= 0)
	{
		//Generate the container material component (used by mesh too)
		ComponentMaterial* comp_material = (ComponentMaterial*)container->CreateComponent(COMPONENT_TYPE::COMP_MATERIAL);

		//Get the material
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		//Load Normal data
		ImportMaterialTextures(material, aiTextureType_HEIGHT, comp_material);

		//Load diffuse data
		ImportMaterialTextures(material, aiTextureType_DIFFUSE, comp_material);

		//Load Specular data
		ImportMaterialTextures(material, aiTextureType_SPECULAR, comp_material);

		LOG("- Material with index %i loaded!", mesh->mMaterialIndex);

		//Set mesh material
		comp_mesh->SetDrawMaterial(comp_material);
	}

	if (!found)
	{
		//Import the mesh (own format)
		App->importer->mesh_importer.Import(name, indices, vertices);

		//Generate the mesh resource
		resource_mesh->SetOriginalFile(file_path.c_str());
		char f_name[100];
		sprintf(f_name, "%s.fiesta", name);
		resource_mesh->SetOwnFile(f_name);

		//Save the meta file
		resource_mesh->Save();
		
		//Insert the loaded mesh 
		loaded_meshes.insert(std::pair<uint, aiMesh*>(resource_mesh->GetID(), mesh));
	}

	//Clear the used containers
	vertices.clear();
	indices.clear();
	textures.clear();
	vertices_pos.clear();
}

void SceneImporter::ImportMaterialTextures(aiMaterial * material, aiTextureType type, ComponentMaterial* container)
{
	if (material->GetTextureCount(type) == 0)return;

	std::map<uint, aiMaterial*>::const_iterator item = loaded_materials.begin();
	while (item != loaded_materials.end())
	{
		if (item._Ptr->_Myval.second == material)
		{
			ResourceMaterial* m_res = (ResourceMaterial*)App->res_manager->Find(item._Ptr->_Myval.first);

			if (m_res != nullptr && StrToAiTextureType(m_res->GetMaterialType()) == type)
			{
				container->AddTexture(m_res);
			}
			return;
		}
	}

	n_textures.clear();
	_textures.clear();

	for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString str;
		material->GetTexture(type, i, &str);
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
			usable_str_b = cur_path;
			usable_str_b += texture.path.c_str();
			App->importer->ImportFile(usable_str_b.c_str());

			//Find the loaded mat and add the id on the mesh materials
			ResourceMaterial* mat = (ResourceMaterial*)App->res_manager->Find(usable_str_b.c_str(),RESOURCE_TYPE::MATERIAL_RESOURCE);
			mat->SetMaterialType(AiTextureTypeToStr(type));

			if (mat != nullptr)
			{
				container->AddTexture(mat, false);
				//Track the loaded material
				loaded_materials.insert(std::pair<uint, aiMaterial*>(mat->GetID(), material));
			}
		}
	}

	return;
}

const char* SceneImporter::AiTextureTypeToStr(aiTextureType ty)
{
	switch (ty)
	{
	case aiTextureType_DIFFUSE:		return "diffuse";
	case aiTextureType_SPECULAR:	return "specular";
	case aiTextureType_AMBIENT:		return "ambient";
	case aiTextureType_EMISSIVE:	return "emissive";
	case aiTextureType_HEIGHT:		return "height";
	case aiTextureType_NORMALS:		return "normals";
	case aiTextureType_SHININESS:	return "shininess";
	case aiTextureType_OPACITY:		return "opacity";
	case aiTextureType_DISPLACEMENT:return "displacement";
	case aiTextureType_LIGHTMAP:	return "lightmap";
	case aiTextureType_REFLECTION:	return "reflection";
	case aiTextureType_UNKNOWN:		return "unknown";
	}

	return "none";
}

aiTextureType SceneImporter::StrToAiTextureType(const char* str)
{
	if (strcmp(str, "diffuse") == 0)return aiTextureType_DIFFUSE;
	if (strcmp(str, "specular") == 0)return aiTextureType_SPECULAR;
	if (strcmp(str, "ambient") == 0)return aiTextureType_AMBIENT;
	if (strcmp(str, "emissive") == 0)return aiTextureType_EMISSIVE;
	if (strcmp(str, "height") == 0)return aiTextureType_HEIGHT;
	if (strcmp(str, "normals") == 0)return aiTextureType_NORMALS;
	if (strcmp(str, "shininess") == 0)return aiTextureType_SHININESS;
	if (strcmp(str, "opacity") == 0)return aiTextureType_OPACITY;
	if (strcmp(str, "displacement") == 0)return aiTextureType_DISPLACEMENT;
	if (strcmp(str, "lightmap") == 0)return aiTextureType_LIGHTMAP;
	if (strcmp(str, "reflection") == 0)return aiTextureType_REFLECTION;
	if (strcmp(str, "unknown") == 0)return aiTextureType_UNKNOWN;

	return aiTextureType_NONE;
}

