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
	/*//Calculate the number of vertex
	num_vertices = 36 * divisions;

	//Get geometry triangulation
	std::vector<math::float3> all_vertex;
	all_vertex.reserve(num_vertices);
	geometry->Triangulate(divisions, divisions, divisions, all_vertex.data(), NULL, NULL, true);
	
	//Set vertex vector
	for (uint k = 0; k < num_vertices; k++)
	{
		n_vertices.push_back(Vertex(all_vertex.data()[k], math::float3(0, 0, 0), math::float2(0, 0)));
	}
	

	//Generate the index
	std::vector<GLuint> my_index;
	std::vector<math::float3> my_vertex;
	VertexToIndex(all_vertex.data(), num_vertices, &my_index, &my_vertex);
	num_indices = my_index.size();

	//Set indices vector
	//n_indices = my_index;

	//Save geometry vertex in a generic buffer
	glGenBuffers(1, (GLuint*)&(id_vertices));
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 3, my_vertex.data(), GL_STATIC_DRAW);

	//Save geometry index in a buffer of elements
	glGenBuffers(1, (GLuint*)&(id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * num_indices, my_index.data(), GL_STATIC_DRAW);*/

	//all_vertex.clear();

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
