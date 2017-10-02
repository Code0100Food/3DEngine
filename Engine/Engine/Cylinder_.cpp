#include "Cylinder_.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include "MathGeoLib/Geometry/Cylinder.h"

Cylinder_::Cylinder_() :Primitive_(PRIMITIVE_TYPE::PRIMITIVE_CYLINDER)
{
	geometry = new math::Cylinder();
}

Cylinder_::Cylinder_(const Cylinder_ & _cpy) :Primitive_(_cpy)
{
	geometry = new math::Cylinder(*_cpy.geometry);
}

Cylinder_::~Cylinder_()
{
	RELEASE(geometry);
}

void Cylinder_::Initialize()
{
	//Get all the vertex of the triangulation
	std::vector<math::float3> vertex = geometry->Triangulate(divisions);
	mesh.num_vertices = vertex.size();

	//Generate the index with the triangulated vertex
	std::vector<GLuint> my_index;
	std::vector<math::float3> my_vertex;
	VertexToIndex(vertex.data(), mesh.num_vertices, &my_index, &my_vertex);
	mesh.num_indices = my_index.size();
	

	//Save geometry vertex in a generic buffer
	glGenBuffers(1, (GLuint*)&(mesh.id_vertices));
	glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * my_vertex.size() * 3, my_vertex.data(), GL_STATIC_DRAW);

	//Save geometry index in a buffer of elements
	glGenBuffers(1, (GLuint*)&(mesh.id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.num_indices, my_index.data(), GL_STATIC_DRAW);

}
