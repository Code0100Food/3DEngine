#include "CylinderGenerator.h"

// Constructors =================================
CylinderGenerator::CylinderGenerator()
{

}

// Destructors ==================================
CylinderGenerator::~CylinderGenerator()
{

}

// Game Loop ====================================
std::pair<std::vector<uint>, std::vector<Vertex>> CylinderGenerator::Generate()
{
	//Get all the vertex of the triangulation
	std::vector<math::float3> vertex = geometry.Triangulate(divisions);
	uint num_vertices = vertex.size();
	std::vector<Vertex> vec_all;
	for (uint k = 0; k < vertex.size(); k++)vec_all.push_back(Vertex(vertex[k], math::float3(0, 0, 0), math::float2(0, 0)));

	//Generate the index with the triangulated vertex
	std::vector<Vertex> vec_t;
	std::vector<uint> my_index;
	std::vector<math::float3> my_vertex;
	VertexToIndex(vec_all.data(), num_vertices, &my_index, &vec_t);
	
	return { my_index,vec_t };
}

void CylinderGenerator::SetTop(math::float3 top)
{
	geometry.l.b = top;
}

void CylinderGenerator::SetBottom(math::float3 bottom)
{
	geometry.l.a = bottom;
}

void CylinderGenerator::SetRad(uint rad)
{
	geometry.r = rad;
}
