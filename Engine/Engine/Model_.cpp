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
#include "ModuleRenderer3D.h"
#include "GeometryManager.h"
#include "ModuleAudio.h"

// Constructors =================================
Model_::Model_(const char * path, bool& check)
{
	check = LoadModel(path);
}

// Destructors ==================================
Model_::~Model_()
{
	uint size = meshes.size();
	for (uint k = 0; k < size; k++)
	{
		meshes[k].ReleaseBuffers();
	}
	meshes.clear();

	size = textures.size();
	for (uint k = 0; k < size; k++)
	{
		glDeleteBuffers(1, (GLuint*)&textures[k].id);
	}
	textures.clear();
	
	bounding_box.clear();
}

// Game Loop ====================================
void Model_::Draw()
{
	if (render_flags & REND_BOUND_BOX)DrawBoundingBox();

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw();
	}
}

// Functionality ================================
bool Model_::LoadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		LOG("[error] ASSIMP: %s", import.GetErrorString());
		return false;
	}
	
	directory = path.substr(0, path.find_last_of('\\'));

	//Get root node transformation
	this->SetTransformation(scene->mRootNode->mTransformation);
	this->name = scene->mRootNode->mName.C_Str();
	LOG("Loading %s model!", this->name.c_str());

	LOG("Loading %i meshes...", scene->mNumMeshes);
	
	ProcessNode(scene->mRootNode, scene);

	GenerateBoundingBox();

	return true;
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
		
		num_tris += n_mesh.num_tris;
		num_vertex += n_mesh.num_vertex;

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
			texture.id = App->textures->LoadTexture(str.C_Str(), directory.c_str());
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

void Model_::DrawBoundingBox() const
{
	//Draw bounding box
	App->renderer3D->DisableGLRenderFlags();
	glBegin(GL_LINES);

	glColor4f(App->geometry->bounding_box_color[0], App->geometry->bounding_box_color[1], App->geometry->bounding_box_color[2], App->geometry->bounding_box_color[3]);
	glLineWidth(2.f);

	for (uint k = 0; k < 4; k++)
	{
		glVertex3f(bounding_box.data()[k + 4].x, bounding_box.data()[k + 4].y, bounding_box.data()[k + 4].z);
		glVertex3f(bounding_box.data()[k].x, bounding_box.data()[k].y, bounding_box.data()[k].z);
	}

	for (uint k = 0; k <= 4; k += 4)
	{
		glVertex3f(bounding_box.data()[k].x, bounding_box.data()[k].y, bounding_box.data()[k].z);
		glVertex3f(bounding_box.data()[k + 1].x, bounding_box.data()[k + 1].y, bounding_box.data()[k + 1].z);

		glVertex3f(bounding_box.data()[k + 2].x, bounding_box.data()[k + 2].y, bounding_box.data()[k + 2].z);
		glVertex3f(bounding_box.data()[k + 3].x, bounding_box.data()[k + 3].y, bounding_box.data()[k + 3].z);

		glVertex3f(bounding_box.data()[k].x, bounding_box.data()[k].y, bounding_box.data()[k].z);
		glVertex3f(bounding_box.data()[k + 2].x, bounding_box.data()[k + 2].y, bounding_box.data()[k + 2].z);

		glVertex3f(bounding_box.data()[k + 1].x, bounding_box.data()[k + 1].y, bounding_box.data()[k + 1].z);
		glVertex3f(bounding_box.data()[k + 3].x, bounding_box.data()[k + 3].y, bounding_box.data()[k + 3].z);
	}
	glEnd();
	App->renderer3D->EnableGLRenderFlags();
}

// Get Methods ==================================
const char * Model_::GetName() const
{
	return name.c_str();
}

math::float3 Model_::GetPosition() const
{
	return math::float3(position.x, position.y, position.z);
}

float Model_::GetFocusDistance() const
{
	if (bounding_box.data() == NULL)return 0.0f;
	math::float3 vec(bounding_box.data()[0].x - bounding_box.data()[7].x, bounding_box.data()[0].y - bounding_box.data()[7].y, bounding_box.data()[0].z - bounding_box.data()[7].z);
	
	return vec.Length();
}

// Set Methods ==================================
void Model_::SetTransformation(aiMatrix4x4 mat)
{
	transformation = mat;
	mat.Decompose(scale, rotation, position);
}

void Model_::SetRenderFlags(RENDER_FLAGS n_flag)
{
	render_flags = n_flag;
}

// Functionality ================================
void Model_::GenerateBoundingBox()
{
	math::float3 min_pos(0, 0, 0);
	math::float3 max_pos(0, 0, 0);

	uint m_size = meshes.size();
	for (uint h = 0; h < m_size; h++)
	{
		uint bound_size = meshes[h].bounding_box.size();
		for (uint k = 0; k < bound_size; k++)
		{
			min_pos.x = MIN(min_pos.x, meshes[h].bounding_box[k].x);
			min_pos.y = MIN(min_pos.y, meshes[h].bounding_box[k].y);
			min_pos.z = MIN(min_pos.z, meshes[h].bounding_box[k].z);

			max_pos.x = MAX(max_pos.x, meshes[h].bounding_box[k].x);
			max_pos.y = MAX(max_pos.y, meshes[h].bounding_box[k].y);
			max_pos.z = MAX(max_pos.z, meshes[h].bounding_box[k].z);
		}
	}

	//Save the vertex of the built bounding box
	math::AABB b_b;
	b_b.minPoint = min_pos;
	b_b.maxPoint = max_pos;
	std::vector<math::float3> temp_vec;
	temp_vec.reserve(8);
	b_b.GetCornerPoints(temp_vec.data());
	for (uint k = 0; k < 8; k++)bounding_box.push_back(temp_vec.data()[k]);
	temp_vec.clear();
}

void Model_::BlitInfo()
{
	//Header of the model
	bool ui_state = ui_opened;
	if (ImGui::CollapsingHeader(("%s", name.c_str()), NULL))
	{
		ui_opened = true;
		if (ui_state != ui_opened)
		{
			App->audio->PlayFxForInput(CHECKBOX_FX);
			ui_state = ui_opened;
		}

		ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Transformation");

		//Show mesh position
		ImGui::Text("Position	");
		ImGui::Text("X %.1f		", position.x);
		ImGui::SameLine();
		ImGui::Text("Y %.1f		", position.y);
		ImGui::SameLine();
		ImGui::Text("Z %.1f", position.z);

		float eX = atan2(-2 * (rotation.y*rotation.z - rotation.w*rotation.x), rotation.w*rotation.w - rotation.x*rotation.x - rotation.y*rotation.y + rotation.z*rotation.z);
		float eY = asin(2 * (rotation.x*rotation.z + rotation.w*rotation.y));
		float eZ = atan2(-2 * (rotation.x*rotation.y - rotation.w*rotation.z), rotation.w*rotation.w + rotation.x*rotation.x - rotation.y*rotation.y - rotation.z*rotation.z);

		//Show mesh rotation
		ImGui::Text("Rotation	");
		ImGui::Text("X %.1f     ", eX);
		ImGui::SameLine();
		ImGui::Text("Y %.1f     ", eY);
		ImGui::SameLine();
		ImGui::Text("Z %.1f", eZ);

		//Show mesh scale
		ImGui::Text("Scale	");
		ImGui::Text("X %.1f     ", scale.x);
		ImGui::SameLine();
		ImGui::Text("Y %.1f     ", scale.y);
		ImGui::SameLine();
		ImGui::Text("Z %.1f", scale.z);

		ImGui::Separator();

		//Show model Tris & Vertex
		ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Tris & Vertex");
		ImGui::Text("Tris: %i", num_tris);
		ImGui::SameLine();
		ImGui::Text("Vertex: %i", num_vertex);

		ImGui::Separator();
		
		//Show model render flags
		ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Render Flags");
		bool rend_b_b = bool(render_flags & REND_BOUND_BOX);
		if (ImGui::Checkbox("Bounding Box", &rend_b_b))
		{
			if (rend_b_b)
			{
				render_flags |= REND_BOUND_BOX;
			}
			else
			{
				render_flags &= ~REND_BOUND_BOX;
			}
			App->audio->PlayFxForInput(FX_ID::CHECKBOX_FX);
		}

		//Iterate all the meshes to blit the info
		uint size = meshes.size();
		for (uint k = 0; k < size; k++)
		{
			meshes[k].BlitInfo(k);
		}
	}
	else
	{
		ui_opened = false;
		if (ui_state != ui_opened)
		{
			ui_state = ui_opened;
			App->audio->PlayFxForInput(CHECKBOX_FX);
		}
	}
}
