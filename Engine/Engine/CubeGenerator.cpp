#include "CubeGenerator.h"

// Constructors =================================
CubeGenerator::CubeGenerator()
{

}

// Destructors ==================================
CubeGenerator::~CubeGenerator()
{

}

// Game Loop ====================================
std::pair<std::vector<uint>, std::vector<Vertex>> CubeGenerator::Generate()
{
	//Get geometry triangulation
	math::float3 all_vertex[144];
	math::float3 all_normals[144];
	math::float2 all_uvs[144];

	CubeGenerator* tar = this;
	geometry.Triangulate(2, 2, 2, all_vertex, all_normals, all_uvs, false);
	std::vector<Vertex> vec_all;
	for (uint k = 0; k < 144; k++)vec_all.push_back(Vertex(all_vertex[k], all_normals[k], all_uvs[k]));

	//Generate the index
	std::vector<Vertex> vec_t;
	std::vector<uint> my_index;
	VertexToIndex(vec_all.data(), 144, &my_index, &vec_t);

	return { my_index,vec_t };
}

// Set Methods ==================================
void CubeGenerator::SetMinPoint(math::float3 minp)
{
	geometry.minPoint = minp;
}

void CubeGenerator::SetMaxPoint(math::float3 maxp)
{
	geometry.maxPoint = maxp;
}
