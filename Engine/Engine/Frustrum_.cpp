#include "Frustrum_.h"
#include "MathGeoLib/Geometry/Frustum.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

Frustrum_::Frustrum_() : Primitive_(PRIMITIVE_FRUSTUM)
{
	geometry = new math::Frustum();
}

Frustrum_::Frustrum_(const Frustrum_ & cpy) : Primitive_(cpy)
{
	geometry = new math::Frustum(*cpy.geometry);
}

Frustrum_::~Frustrum_()
{
	if (geometry)
	{
		RELEASE(geometry);
	}
}

void Frustrum_::Initialize()
{
	/*//Get Triangles
	math::float3* all_vertex = (math::float3*)malloc(sizeof(math::float3) * 36);
	geometry->Triangulate(all_vertex);

	//Get indexs
	std::vector<GLuint> indexs;
	std::vector<math::float3> vertex;

	VertexToIndex(all_vertex, 36, &indexs, &vertex);
	
	//Save geometry vertexs
	glGenBuffers(1, (GLuint*)&(vertex_buffer_id));
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex.size() * 3, vertex.data(), GL_STATIC_DRAW);

	//Save Index Values
	glGenBuffers(1, (GLuint*)&(index_buffer_id));
	glBindBuffer(GL_ARRAY_BUFFER, index_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * indexs.size() * 3, indexs.data(), GL_STATIC_DRAW);*/

}

void Frustrum_::Draw()
{
	/*// The geometry can't be drawn if is not allocated
	if (index_buffer_id == 0 || vertex_buffer_id == 0)return;

	//Open focus of index (elements)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
	//Open focus index of vertex (data)
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
	//Define how to read the vertex buffer
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//Draw the defined index interpreting the vertex of the data buffer with the defined mode
	glDrawElements(GL_TRIANGLES, sizeof(GLuint) * 24 * pow(4, divisions), GL_UNSIGNED_INT, NULL);*/
}
