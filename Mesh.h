#ifndef MESH_H
#define MESH_H

#define _CRT_SECURE_NO_WARNINGS

#include <glm/glm.hpp>
#include <glew.h>

#include <vector>

enum class MeshType {Mesh, MeshNormals, MeshTextured, MeshNormalsTextured, Count};

class Mesh
{
protected:
	GLuint VBO, VAO, EBO;
	MeshType meshType;
	std::vector<glm::vec3> vertices;
public:
	glm::vec3 position;
	int specLevel;
	std::vector<GLuint> indices;
	Mesh();
	virtual void loadFromFile(const char* fileName);
	virtual void setup();
	int getVAO();
	MeshType getType();
};

#endif

