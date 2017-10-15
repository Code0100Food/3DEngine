#include "MeshImporter.h"
#include "Application.h"
#include "FileSystem.h"

MeshImporter::MeshImporter(const char* name, std::vector<math::float3> vertices, std::vector<uint> indices)
{
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
	bytes = sizeof(ranges);

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
}
