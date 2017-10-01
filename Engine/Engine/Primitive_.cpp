#include "Primitive_.h"

// Constructors =================================
Primitive_::Primitive_(PRIMITIVE_TYPE _type):type(type)
{

}

Primitive_::Primitive_(const Primitive_ & _cpy) :color(_cpy.color), axis(_cpy.axis), type(_cpy.type)
{

}

// Destructors ==================================
Primitive_::~Primitive_()
{

}

// Set Methods ==================================
void Primitive_::SetColor(const Color & n_color)
{
	color = n_color;
}

void Primitive_::SetAxis(bool n_axis)
{
	axis = n_axis;
}

void Primitive_::SetType(PRIMITIVE_TYPE n_type)
{
	type = n_type;
}

void Primitive_::SetDivisions(uint def)
{
	divisions = def;
}

// Get Methods ==================================
Color Primitive_::GetColor() const
{
	return color;
}

bool Primitive_::GetAxis() const
{
	return axis;
}

PRIMITIVE_TYPE Primitive_::GetType() const
{
	return type;
}

bool VertexToIndex(math::float3* all_vertex,uint vertex_num, std::vector<uint>* index, std::vector<math::float3>* vertex)
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
