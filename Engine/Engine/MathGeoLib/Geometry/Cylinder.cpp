#include "Cylinder.h"
#include "../../Globals.h"

MATH_BEGIN_NAMESPACE

Cylinder::Cylinder()
{

}

Cylinder::Cylinder(const Cylinder & _cpy) :r(_cpy.r), l(_cpy.l)
{

}

Cylinder::~Cylinder()
{

}

std::vector<math::float3> Cylinder::Triangulate(int slices) const
{
	std::vector<math::float3> vertex_vec;

	double i;
	float ang = 360.0f / slices;
	double resolution = ang * DEGTORAD;

	double height = l.Length();

	for (i = 0; i <= 2 * HAVE_M_PI; i += resolution)
	{
		vertex_vec.push_back(math::float3(0, height, 0));
		vertex_vec.push_back(math::float3(r * cos(i + resolution), height, r * sin(i + resolution)));
		vertex_vec.push_back(math::float3(r * cos(i), height, r * sin(i)));
	}

	for (i = 2 * HAVE_M_PI; i >= 0; i -= resolution)
	{
		vertex_vec.push_back(math::float3(r * cos(i), 0, r * sin(i)));
		vertex_vec.push_back(math::float3(r * cos(i + resolution), 0, r * sin(i + resolution)));
		vertex_vec.push_back(math::float3(0, 0, 0));
	}

	for (i = 0; i <= 2 * HAVE_M_PI; i += resolution)
	{
		vertex_vec.push_back(math::float3(r * cos(i), height, r * sin(i)));
		vertex_vec.push_back(math::float3(r * cos(i + resolution), height, r * sin(i + resolution)));
		vertex_vec.push_back(math::float3(r * cos(i + resolution), 0, r * sin(i + resolution)));
	}

	for (i = 2 * HAVE_M_PI; i >= 0; i -= resolution)
	{
		vertex_vec.push_back(math::float3(r * cos(i), height, r * sin(i)));
		vertex_vec.push_back(math::float3(r * cos(i + resolution), 0, r * sin(i + resolution)));
		vertex_vec.push_back(math::float3(r * cos(i), 0, r * sin(i)));
	}

	return vertex_vec;
}

MATH_END_NAMESPACE