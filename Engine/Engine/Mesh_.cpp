#include "Mesh_.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include "Primitive.h"

Mesh_::Mesh_()
{

}

Mesh_::Mesh_(std::vector<Vertex> vertices, std::vector<uint> indices, std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	this->SetupMesh();
}

Mesh_::~Mesh_()
{

}

void Mesh_::Draw()
{

	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
										  // retrieve texture number (the N in diffuse_textureN)
		/*std::stringstream ss;
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			ss << diffuseNr++; // transfer unsigned int to stream
		else if (name == "texture_specular")
			ss << specularNr++; // transfer unsigned int to stream
		number = ss.str();

		shader.setFloat(("material." + name + number).c_str(), i);*/
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// Draw the mesh
	glBindVertexArray(VertexArrayObjects);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBegin(GL_LINES);
	for (uint k = 0; k < vertices.size(); k++)
	{	
		glColor3f(0.5f, 0.8f, 0.8f);
		glVertex3f(vertices[k].position.x, vertices[k].position.y, vertices[k].position.z);
		glColor3f(0.3f, 1.0f, 1.0f);
		glVertex3f(vertices[k].position.x + vertices[k].normal.x, vertices[k].position.y + vertices[k].normal.y, vertices[k].position.z + vertices[k].normal.z);
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();
}

void Mesh_::SetupMesh()
{
	glGenVertexArrays(1, &VertexArrayObjects);
	glGenBuffers(1, &VertexBufferObjects);
	glGenBuffers(1, &ElementBufferObjects);

	glBindVertexArray(VertexArrayObjects);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObjects);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObjects);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}