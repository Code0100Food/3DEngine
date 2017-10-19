#include "Sphere_.h"
#include "MathGeoLib/Geometry/Sphere.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <math.h>
#include "MathGeoLib/Math/float2.h"
#include "MathGeoLib/Math/float3.h"
#include "Primitive_.h"

// Constructors =================================
Sphere_::Sphere_()
{
	
}

Sphere_::Sphere_(const Sphere_ & _cpy) :geometry(_cpy.geometry), divisions(_cpy.divisions)
{
	
}

// Destructors ==================================
Sphere_::~Sphere_()
{

}

// Game Loop ====================================
std::pair<std::vector<uint>, std::vector<Vertex>>  Sphere_::Initialize()
{

	//Calculate the number of vertex
	uint num_vertices = 24 * pow(4,divisions);

	//Get geometry triangulation
	std::vector<math::float3> all_vertex;
	all_vertex.reserve(num_vertices);
	std::vector<math::float3> all_normals;
	all_normals.reserve(num_vertices);

	//Triangulate the sphere geometry
	geometry.Triangulate(all_vertex.data(), all_normals.data(), NULL, num_vertices, true);

	
	// Get the correct vertex and index of the triangulation
	std::vector<Vertex> vec_t;
	std::vector<uint> my_index;
	std::vector<math::float3> my_vertex;
	VertexToIndex(all_vertex.data(), num_vertices, &my_index, &my_vertex);
	for (uint k = 0; k < my_vertex.size(); k++)vec_t.push_back(Vertex(my_vertex.data()[k], math::float3(0, 0, 0), math::float2(0, 0)));

	return{ my_index,vec_t };
}

// Set Methods ==================================
void Sphere_::SetPosition(math::float3 pos)
{
	geometry.pos = pos;
}

void Sphere_::SetRad(float rad)
{
	geometry.r = rad;
}

void Sphere_::SetDivisions(uint div)
{
	divisions = div;
}
