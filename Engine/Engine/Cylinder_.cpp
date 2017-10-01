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

}

void Cylinder_::Initialize()
{
	std::vector<math::float3> vertex = geometry->Triangulate(divisions);
	vertex_num = vertex.size();
	std::vector<GLuint> my_index;
	std::vector<math::float3> my_vertex;

	VertexToIndex(vertex.data(),vertex_num, &my_index, &my_vertex);

	//Save geometry vertex in a generic buffer
	glGenBuffers(1, (GLuint*)&(vertex_buffer_id));
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * my_vertex.size() * 3, my_vertex.data(), GL_STATIC_DRAW);

	//Save geometry index in a buffer of elements
	glGenBuffers(1, (GLuint*)&(index_buffer_id));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vertex_num, my_index.data(), GL_STATIC_DRAW);

}

void Cylinder_::Draw()
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
	glDrawElements(GL_TRIANGLES, sizeof(GLuint) * vertex_num, GL_UNSIGNED_INT, NULL);
}
