#include "Cube_.h"
#include "MathGeoLib/Geometry/AABB.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"


// Constructors =================================
Cube_::Cube_() :Primitive_(PRIMITIVE_CUBE)
{
	geometry = new math::AABB();
}

Cube_::Cube_(const Cube_ & _cpy): Primitive_(_cpy)
{
	geometry = new math::AABB(*_cpy.geometry);
}

// Destructors ==================================
Cube_::~Cube_()
{
	RELEASE(geometry);
}

// Game Loop ====================================
void Cube_::Initialize()
{
	//Calculate the number of vertex
	const uint vertex_num = 36 * divisions;

	//Get geometry vertex
	math::float3 vertex[16];
	geometry->GetCornerPoints(vertex);
	
	//Get geometry triangulation
	math::float3* all_vertex = (math::float3*)malloc(sizeof(math::float3) * vertex_num);
	geometry->Triangulate(divisions, divisions, divisions, all_vertex, NULL, NULL, true);

	//Generate the index
	
	GLuint* my_index = (GLuint*)malloc(sizeof(GLuint) * vertex_num);

	for (uint k = 0; k < vertex_num; k++)
	{
		for (uint x = 0; x < 8; x++)
		{
			if (all_vertex[k] == vertex[x])
			{
				my_index[k] = x;
			}
		}
	}

	//Save geometry vertex in a generic buffer
	glGenBuffers(1, (GLuint*)&(vertex_buffer_id));
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 3, vertex, GL_STATIC_DRAW);

	//Save geometry index in a buffer of elements
	glGenBuffers(1, (GLuint*)&(index_buffer_id));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vertex_num, my_index, GL_STATIC_DRAW);

	//RELEASE_ARRAY(my_index);
}

void Cube_::Draw()
{
	//The geometry can't be drawn if is not allocated
	if (index_buffer_id == 0 || vertex_buffer_id == 0)return;

	//Open focus of index (elements)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
	//Open focus index of vertex (data)
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
	//Define how to read the vertex buffer
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//Draw the defined index interpreting the vertex of the data buffer with the defined mode
	glDrawElements(GL_TRIANGLES, sizeof(GLuint) * 36 * divisions, GL_UNSIGNED_INT, NULL);
}

// Set Methods ==================================
void Cube_::SetMinPoint(math::float3 minp)
{
	geometry->minPoint = minp;
}

void Cube_::SetMaxPoint(math::float3 maxp)
{
	geometry->maxPoint = maxp;
}
