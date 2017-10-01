#include "Sphere_.h"
#include "MathGeoLib/Geometry/Sphere.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <math.h>

// Constructors =================================
Sphere_::Sphere_():Primitive_(PRIMITIVE_SPHERE)
{
	geometry = new math::Sphere();
}

Sphere_::Sphere_(const Sphere_ & _cpy) :Primitive_(_cpy)
{
	geometry = new math::Sphere(*_cpy.geometry);
}

// Destructors ==================================
Sphere_::~Sphere_()
{
	RELEASE(geometry);
}

// Game Loop ====================================
void Sphere_::Initialize()
{
	//Calculate the number of vertex
	const uint vertex_num = 24 * pow(4,divisions);

	//Get geometry triangulation
	math::float3* all_vertex = (math::float3*)malloc(sizeof(math::float3) * vertex_num);
	geometry->Triangulate(all_vertex, NULL, NULL, vertex_num, true);

	std::vector<GLuint> my_index;
	std::vector<math::float3> my_vertex;

	VertexToIndex(all_vertex, vertex_num, &my_index, &my_vertex);

	//Save geometry vertex in a generic buffer
	glGenBuffers(1, (GLuint*)&(vertex_buffer_id));
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * my_vertex.size() * 3, my_vertex.data(), GL_STATIC_DRAW);

	//Save geometry index in a buffer of elements
	glGenBuffers(1, (GLuint*)&(index_buffer_id));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vertex_num, my_index.data(), GL_STATIC_DRAW);

	//RELEASE(all_vertex);

}

void Sphere_::Draw()
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
	glDrawElements(GL_TRIANGLES, sizeof(GLuint) * 24 * pow(4, divisions), GL_UNSIGNED_INT, NULL);
}

// Set Methods ==================================
void Sphere_::SetPosition(math::float3 pos)
{
	geometry->pos = pos;
}

void Sphere_::SetRad(float rad)
{
	geometry->r = rad;
}
