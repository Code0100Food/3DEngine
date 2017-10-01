#include "Capsule_.h"
#include "MathGeoLib/Geometry/Capsule.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

Capsule_::Capsule_() :Primitive_(PRIMITIVE_CAPSULE)
{
	geometry = new math::Capsule();
}

Capsule_::Capsule_(const Capsule_ & _cpy) : Primitive_(_cpy)
{
	geometry = new math::Capsule(*_cpy.geometry);
}

Capsule_::~Capsule_()
{
	RELEASE(geometry);
}

void Capsule_::Initialize()
{
	/*int i, j, index = 0;
	int i1, i2, i3, i4;
	double theta, phi;
	double height = geometry->l.Length();
	MESHVERTEX *p = NULL, n;
	double len;
	FILE *fptr;

	// Create data
	if ((p = malloc((N + 1)*(N / 2 + 2) * sizeof(MESHVERTEX))) == NULL) {
		fprintf(stderr, "malloc failed\n");
		exit(-1);
	}

	for (j = 0; j <= N / 4; j++) { // top cap
		for (i = 0; i <= N; i++) {
			theta = i * TWOPI / N;
			phi = -PID2 + PI * j / (N / 2);
			p[index].x = geometry->r * cos(phi) * cos(theta);
			p[index].y = geometry->r * cos(phi) * sin(theta);
			p[index].z = geometry->r * sin(phi);
			p[index].nx = p[index].x;
			p[index].ny = p[index].y;
			p[index].nz = p[index].z;
			p[index].z -= height / 2;
			index++;
		}
	}
	for (j = N / 4; j <= N / 2; j++) { // bottom cap
		for (i = 0; i <= N; i++) {
			theta = i * TWOPI / N;
			phi = -PID2 + PI * j / (N / 2);
			p[index].x = radius * cos(phi) * cos(theta);
			p[index].y = radius * cos(phi) * sin(theta);
			p[index].z = radius * sin(phi);
			p[index].nx = p[index].x;
			p[index].ny = p[index].y;
			p[index].nz = p[index].z;
			p[index].z += height / 2;
			index++;
		}
	}*/

	/*
	if (index != (N + 1)*(N / 2 + 2)) {
		fprintf(stderr, "Unexpeced number of vertices, %d != %d\n", index, (N + 1)*(N / 2 + 2));
		exit(-1);
	}

	// Calculate texture coordinates
	for (i = 0; i<index; i++) {
		p[i].u = atan2(p[i].y, p[i].x) / TWOPI;
		if (p[i].u < 0)
			p[i].u = 1 + p[i].u;
		p[i].v = 0.5 + atan2(p[i].z, sqrt(p[i].x*p[i].x + p[i].y*p[i].y)) / PI;
	}

	// Save as textured obj file
	fptr = fopen("capsule.obj", "w");
	fprintf(fptr, "mtllib capsule.mtl\n");
	// Vertices
	for (i = 0; i<index; i++)
		fprintf(fptr, "v %lf %lf %lf\n", p[i].x, p[i].y, p[i].z);
	// Normals (same as vertices in this case but normalised)
	for (i = 0; i<index; i++) {
		n = p[i];
		len = sqrt(n.x*n.x + n.y*n.y + n.z*n.z);
		fprintf(fptr, "vn %lf %lf %lf\n", n.x / len, n.y / len, n.z / len);
	}
	// Texture coordinates
	for (i = 0; i<index; i++)
		fprintf(fptr, "vt %lf %lf\n", p[i].u, p[i].v);
	// Faces, each a triangle
	fprintf(fptr, "usemtl material0\n");
	for (j = 0; j <= N / 2; j++) {
		for (i = 0; i<N; i++) {
			i1 = j    * (N + 1) + i + 1;
			i2 = j    * (N + 1) + (i + 1) + 1;
			i3 = (j + 1) * (N + 1) + (i + 1) + 1;
			i4 = (j + 1) * (N + 1) + i + 1;
			fprintf(fptr, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", i1, i1, i1, i2, i2, i2, i3, i3, i3);
			fprintf(fptr, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", i1, i1, i1, i3, i3, i3, i4, i4, i4);
		}
	}

	std::vector<math::float3> vertex = geometry->Triangulate(divisions);
	vertex_num = vertex.size();
	std::vector<GLuint> my_index;
	std::vector<math::float3> my_vertex;

	VertexToIndex(vertex.data(), vertex_num, &my_index, &my_vertex);
	*/

	/*//Save geometry vertex in a generic buffer
	glGenBuffers(1, (GLuint*)&(vertex_buffer_id));
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * my_vertex.size() * 3, my_vertex.data(), GL_STATIC_DRAW);

	//Save geometry index in a buffer of elements
	glGenBuffers(1, (GLuint*)&(index_buffer_id));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vertex_num, my_index.data(), GL_STATIC_DRAW);*/
}

void Capsule_::Draw()
{
	/*//The geometry can't be drawn if is not allocated
	if (index_buffer_id == 0 || vertex_buffer_id == 0)return;

	//Open focus of index (elements)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
	//Open focus index of vertex (data)
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
	//Define how to read the vertex buffer
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//Draw the defined index interpreting the vertex of the data buffer with the defined mode
	glDrawElements(GL_TRIANGLES, sizeof(GLuint) * vertex_num, GL_UNSIGNED_INT, NULL);	*/
}
