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
	num_vertices = 24 * pow(4,divisions);

	//Get geometry triangulation
	std::vector<math::float3> all_vertex;
	all_vertex.reserve(num_vertices);

	geometry->Triangulate(all_vertex.data(), NULL, NULL, num_vertices, true);

	std::vector<GLuint> my_index;
	std::vector<math::float3> my_vertex;

	VertexToIndex(all_vertex.data(), num_vertices, &my_index, &my_vertex);
	num_indices = my_index.size();
	

	//Save geometry vertex in a generic buffer
	glGenBuffers(1, (GLuint*)&(id_vertices));
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * my_vertex.size() * 3, my_vertex.data(), GL_STATIC_DRAW);

	//Save geometry index in a buffer of elements
	glGenBuffers(1, (GLuint*)&(id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * num_indices, my_index.data(), GL_STATIC_DRAW);
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
