#include "MeshImporter.h"
#include "Application.h"
#include "FileSystem.h"
#include "ModuleScene.h"

// Constructors =================================
MeshImporter::MeshImporter()
{

}

// Functionality ================================
bool MeshImporter::Import(const char* name, std::vector<math::float3> vertices, std::vector<uint> indices)
{
	if (name == nullptr || vertices.size() == 0 || indices.size() == 0)return false;

	// amount of indices / vertices / colors / normals / texture_coords / AABB
	uint num_indices = indices.size();
	uint num_vertices = vertices.size();

	//Store the size of the vectors
	uint ranges[2] = { num_indices, num_vertices };
	uint size = sizeof(ranges) + sizeof(uint) * num_indices + sizeof(float) * num_vertices * 3;
	char* data = new char[size]; // Allocate
	char* cur_buffer_pos = data;
	uint bytes = sizeof(ranges);
	memcpy(cur_buffer_pos, ranges, bytes);
	
	// Store indices
	cur_buffer_pos += bytes;
	bytes = sizeof(uint) * num_indices;
	memcpy(cur_buffer_pos, indices.data(), bytes);
	//bytes = sizeof(ranges);

	// Store vertices
	cur_buffer_pos += bytes;
	bytes = sizeof(float) * num_vertices * 3;
	memcpy(cur_buffer_pos, vertices.data(), bytes);

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
		math::float3* vertices_i = new math::float3[num_indices];
		memcpy(vertices_i, cursor, bytes);

		std::vector<Vertex> vertices;
		
		for (uint k = 0; k < num_vertices; k++)vertices.push_back(Vertex(vertices_i[k],math::float3( 0,0,0 ), math::float2(0, 0)));
		mesh_comp->SetVertices(vertices);
		RELEASE_ARRAY(vertices_i);
		
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
