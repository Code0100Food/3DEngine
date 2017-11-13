#include "MeshImporter.h"
#include "Application.h"
#include "FileSystem.h"
#include "ModuleScene.h"
#include "ResourcesManager.h"

// Constructors =================================
MeshImporter::MeshImporter()
{

}

// Functionality ================================
bool MeshImporter::Import(const char* name, std::vector<uint> indices, std::vector<Vertex> vertices)
{
	if (name == nullptr || vertices.size() == 0)return false;

	//Number of indices vertices and textures
	uint num_indices = indices.size();
	uint num_vertices = vertices.size();
	
	//Store the size of the vectors
	uint sizes[2] = { num_indices,num_vertices };
	uint size = sizeof(sizes) + sizeof(uint) * num_indices + sizeof(Vertex) * num_vertices;
	char* data = new char[size]; // Allocate
	char* cur_buffer_pos = data;
	uint bytes = sizeof(sizes);
	memcpy(cur_buffer_pos, sizes, bytes);

	// Store indices
	cur_buffer_pos += bytes;
	bytes = sizeof(uint) * num_indices;
	memcpy(cur_buffer_pos, indices.data(), bytes);

	// Store vertices positions
	cur_buffer_pos += bytes;
	bytes = sizeof(float) * num_vertices * 3;
	std::vector<math::float3> v_pos;
	for (uint k = 0; k < num_vertices; k++)v_pos.push_back(vertices[k].position);
	memcpy(cur_buffer_pos, v_pos.data(), bytes);
	v_pos.clear();

	//Store vertices normals
	cur_buffer_pos += bytes;
	bytes = sizeof(float) * num_vertices * 3;
	std::vector<math::float3> v_norm;
	for (uint k = 0; k < num_vertices; k++)v_norm.push_back(vertices[k].normals);
	memcpy(cur_buffer_pos, v_norm.data(), bytes);
	v_norm.clear();

	//Store vertices texture coords
	cur_buffer_pos += bytes;
	bytes = sizeof(float) * num_vertices * 2;
	std::vector<math::float2> v_tex_coords;
	for (uint k = 0; k < num_vertices; k++)v_tex_coords.push_back(vertices[k].tex_coords);
	memcpy(cur_buffer_pos, v_tex_coords.data(), bytes);
	v_tex_coords.clear();

	//Generate the file name
	std::string file_name = name;
	file_name += ".fiesta";

	//Save the file with all the built data
	App->fs->SaveFile(file_name.c_str(), (char*)data, size, LIBRARY_MESH_FOLDER);

	file_name.clear();
	
	return true;
}

bool MeshImporter::Load(const char * path)
{
	//Load the buffer from the file
	char* buffer;
	App->fs->LoadFile(path, &buffer);

	if (buffer)
	{
		//Create a game object with a mesh component to load the file in
		GameObject* mesh_obj = App->scene->CreateGameObject();
		ComponentMesh* mesh_comp = (ComponentMesh*)mesh_obj->CreateComponent(COMPONENT_TYPE::COMP_MESH);
		ComponentMeshRenderer* mesh_rend_comp = (ComponentMeshRenderer*)mesh_obj->CreateComponent(COMPONENT_TYPE::COMP_MESH_RENDERER);

		//Focus the cursor
		char* cursor = buffer;

		//Load  amount of indices and vertices
		uint ranges[2];
		uint bytes = sizeof(ranges);
		memcpy(ranges, cursor, bytes);
		uint num_indices = ranges[0];
		uint num_vertices = ranges[1];

		// Load indices
		cursor += bytes;
		bytes = sizeof(uint) * num_indices;
		uint* indices_i = new uint[num_indices];
		memcpy(indices_i, cursor, bytes);

		std::vector<uint> indices;
		for (uint k = 0; k < num_indices; k++)indices.push_back(indices_i[k]);
		mesh_comp->SetIndices(indices);
		RELEASE_ARRAY(indices_i);

		//Load vertices
		cursor += bytes;
		bytes = sizeof(math::float3) * num_vertices;
		math::float3* vertices_i = new math::float3[num_vertices];
		memcpy(vertices_i, cursor, bytes);

		//Load normals
		cursor += bytes;
		bytes = sizeof(math::float3) * num_vertices;
		math::float3* normals_i = new math::float3[num_vertices];
		memcpy(normals_i, cursor, bytes);

		//Load texture coords
		cursor += bytes;
		bytes = sizeof(math::float2) * num_vertices;
		math::float2* tex_coords_i = new math::float2[num_vertices];
		memcpy(tex_coords_i, cursor, bytes);

		//Built vertex with the loaded data
		std::vector<Vertex> vertices;
		for (uint k = 0; k < num_vertices; k++)vertices.push_back(Vertex(vertices_i[k], normals_i[k], tex_coords_i[k]));
		mesh_comp->SetVertices(vertices);
		
		//Release all the arrays
		RELEASE_ARRAY(vertices_i);
		RELEASE_ARRAY(normals_i);
		RELEASE_ARRAY(tex_coords_i);

		//Set mesh file path
		std::string name;
		App->fs->GetFileNameFromPath(path, &name);
		mesh_comp->SetPath(name.c_str());

		//Built the mesh buffers
		mesh_comp->SetupMesh();
		
		//Active mesh renderer
		mesh_rend_comp->SetTargetMesh(mesh_comp);
	}
	else
	{
		LOG("[error] Error loading mesh!");
		return false;
	}
	RELEASE_ARRAY(buffer);
	return true;
}

bool MeshImporter::Load(const char * path, ComponentMesh* target)
{
	//Load the buffer from the file
	char* buffer = nullptr;
	App->fs->LoadFile(path, &buffer);

	if (buffer)
	{
		//Create a resource mesh (temporal)
		if (target->MeshResourceIsNull())target->SetResourceMesh((ResourceMesh*)App->res_manager->CreateResource(RESOURCE_TYPE::MESH_RESOURCE));
		
		//Focus the cursor
		char* cursor = buffer;

		//Load  amount of indices and vertices
		uint ranges[2];
		uint bytes = sizeof(ranges);
		memcpy(ranges, cursor, bytes);
		uint num_indices = ranges[0];
		uint num_vertices = ranges[1];

		// Load indices
		cursor += bytes;
		bytes = sizeof(uint) * num_indices;
		uint* indices_i = new uint[num_indices];
		memcpy(indices_i, cursor, bytes);

		std::vector<uint> indices;
		for (uint k = 0; k < num_indices; k++)indices.push_back(indices_i[k]);
		target->SetIndices(indices);
		RELEASE_ARRAY(indices_i);

		//Load vertices
		cursor += bytes;
		bytes = sizeof(math::float3) * num_vertices;
		math::float3* vertices_i = new math::float3[num_vertices];
		memcpy(vertices_i, cursor, bytes);

		//Load normals
		cursor += bytes;
		bytes = sizeof(math::float3) * num_vertices;
		math::float3* normals_i = new math::float3[num_vertices];
		memcpy(normals_i, cursor, bytes);

		//Load texture coords
		cursor += bytes;
		bytes = sizeof(math::float2) * num_vertices;
		math::float2* tex_coords_i = new math::float2[num_vertices];
		memcpy(tex_coords_i, cursor, bytes);

		//Built vertex with the loaded data
		std::vector<Vertex> vertices;
		for (uint k = 0; k < num_vertices; k++)vertices.push_back(Vertex(vertices_i[k], normals_i[k], tex_coords_i[k]));
		target->SetVertices(vertices);

		//Release all the arrays
		RELEASE_ARRAY(vertices_i);
		RELEASE_ARRAY(normals_i);
		RELEASE_ARRAY(tex_coords_i);

		//Set mesh file path
		std::string name;
		App->fs->GetFileNameFromPath(path, &name);
		name = name.substr(0, name.length() - (name.length() - name.find_last_of(".")));
		target->SetPath(name.c_str());

		//Built the mesh buffers
		target->SetupMesh();
	}
	else
	{
		LOG("[error] Error loading mesh!");
		return false;
	}
	RELEASE_ARRAY(buffer);
	return true;
}
