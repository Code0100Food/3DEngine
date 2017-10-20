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
		vertex_vec.push_back(math::float3(l.a.x, l.a.y + height, l.a.z));
		vertex_vec.push_back(math::float3(l.a.x + r * cos(i + resolution), l.a.y + height, l.a.z + r * sin(i + resolution)));
		vertex_vec.push_back(math::float3(l.a.x + r * cos(i), l.a.y + height, l.a.z + r * sin(i)));
	}

	for (i = 2 * HAVE_M_PI; i >= 0; i -= resolution)
	{
		vertex_vec.push_back(math::float3(l.b.x + r * cos(i), l.b.y, l.b.z + r * sin(i)));
		vertex_vec.push_back(math::float3(l.b.x + r * cos(i + resolution), l.b.y, l.b.z + r * sin(i + resolution)));
		vertex_vec.push_back(math::float3(l.b.x, l.b.y, l.b.z));
	}

	for (i = 0; i <= 2 * HAVE_M_PI; i += resolution)
	{
		vertex_vec.push_back(math::float3(l.b.x + r * cos(i), l.a.y + height, l.b.z + r * sin(i)));
		vertex_vec.push_back(math::float3(l.b.x + r * cos(i + resolution), l.a.y + height, l.b.z + r * sin(i + resolution)));
		vertex_vec.push_back(math::float3(l.b.x + r * cos(i + resolution), l.b.y, l.b.z + r * sin(i + resolution)));
	}

	for (i = 2 * HAVE_M_PI; i >= 0; i -= resolution)
	{
		vertex_vec.push_back(math::float3(l.b.x + r * cos(i), l.a.y + height, l.b.z + r * sin(i)));
		vertex_vec.push_back(math::float3(l.b.x + r * cos(i + resolution), l.b.y, l.b.z + r * sin(i + resolution)));
		vertex_vec.push_back(math::float3(l.b.x + r * cos(i), l.b.y, l.b.z + r * sin(i)));
	}

	return vertex_vec;
}

MATH_END_NAMESPACE