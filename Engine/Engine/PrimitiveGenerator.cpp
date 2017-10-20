#include "PrimitiveGenerator.h"

// Constructors =================================
PrimitiveGenerator::PrimitiveGenerator()
{

}

// Destructors ==================================
PrimitiveGenerator::~PrimitiveGenerator()
{

}

void PrimitiveGenerator::SetDivisions(uint div)
{
	divisions = div;
}

bool PrimitiveGenerator::VertexToIndex(Vertex * all_vertex, uint vertex_num, std::vector<uint>* index, std::vector<Vertex>* vertex)
{
	if (index == nullptr || vertex == nullptr || vertex_num == 0)return false;

	std::vector<Vertex> indexed_vertex;
	for (uint k = 0; k < vertex_num; k++)
	{
		uint size = indexed_vertex.size();
		bool found = false;
		for (uint x = 0; x < size; x++)
		{
			if (indexed_vertex[x] == all_vertex[k])
			{
				index->push_back(x);
				found = true;
			}
		}
		if (!found)
		{
			indexed_vertex.push_back(all_vertex[k]);
			index->push_back(indexed_vertex.size() - 1);
			vertex->push_back(all_vertex[k]);
		}
	}

	return true;
}

bool PrimitiveGenerator::VertexToIndex(math::float3 * all_vertex, uint vertex_num, std::vector<uint>* index, std::vector<math::float3>* vertex)
{
	if (index == nullptr || vertex == nullptr || vertex_num == 0)return false;

	std::vector<math::float3> indexed_vertex;
	for (uint k = 0; k < vertex_num; k++)
	{
		uint size = indexed_vertex.size();
		bool found = false;
		for (uint x = 0; x < size; x++)
		{
			if (indexed_vertex[x] == all_vertex[k])
			{
				index->push_back(x);
				found = true;
			}
		}
		if (!found)
		{
			indexed_vertex.push_back(all_vertex[k]);
			index->push_back(indexed_vertex.size() - 1);
			vertex->push_back(all_vertex[k]);
		}
	}

	return true;
}
