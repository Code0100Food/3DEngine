#include "FrustumGenerator.h"

FrustumGenerator::FrustumGenerator()
{

}

FrustumGenerator::~FrustumGenerator()
{

}

std::pair<std::vector<uint>, std::vector<Vertex>> FrustumGenerator::Generate()
{
	geometry.nearPlaneDistance = 1;
	geometry.farPlaneDistance = 15;

	geometry.type = math::PerspectiveFrustum;

	geometry.pos = math::float3(0, 0, 0);
	geometry.front = math::float3(0, 0, 1);
	geometry.up = math::float3(0, 1, 0);

	geometry.orthographicHeight = 5;
	geometry.orthographicWidth = 2;

	geometry.verticalFov = 5;
	geometry.horizontalFov = 8;

	math::float3 all_vertex[9];
	geometry.Triangulate(all_vertex);

	std::vector<Vertex> vec_all;
	for (uint k = 0; k < 9; k++)vec_all.push_back(Vertex(all_vertex[k], math::float3(0, 0, 0), math::float2(0, 0)));

	std::vector<uint> indexs;
	std::vector<math::float3> vertex;

	VertexToIndex(all_vertex, 9, &indexs, &vertex);

	return { indexs ,vec_all };
}
