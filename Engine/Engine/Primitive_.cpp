#include "Primitive_.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

// Constructors =================================
Primitive_::Primitive_(PRIMITIVE_TYPE _type):type(type)
{

}

Primitive_::Primitive_(const Primitive_ & _cpy) : color(_cpy.color), axis(_cpy.axis), type(_cpy.type)
{

}

// Destructors ==================================
Primitive_::~Primitive_()
{

}

// Game Loop ====================================
void Primitive_::Initialize()
{
	//Save geometry vertex in a generic buffer
	glGenBuffers(1, (GLuint*)&(id_vertices));
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vertices, GL_STATIC_DRAW);

	//Save geometry index in a buffer of elements
	glGenBuffers(1, (GLuint*)&(id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * num_indices, indices, GL_STATIC_DRAW);
}

void Primitive_::Draw()
{
	//The geometry can't be drawn if is not allocated
	if (id_indices == 0 || id_vertices == 0)return;

	//Open focus of index (elements)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	//Open focus index of vertex (data)
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	//Define how to read the vertex buffer
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//Draw the defined index interpreting the vertex of the data buffer with the defined mode
	glDrawElements(GL_TRIANGLES, sizeof(GLuint) * num_vertices / 3, GL_UNSIGNED_INT, NULL);

	//Draw the primitive axis
	if (axis)DrawAxis();

}

void Primitive_::DrawAxis() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
	glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
	glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
	glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

	glEnd();

	glLineWidth(1.0f);
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
