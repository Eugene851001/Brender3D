#ifndef MESH_TEXTURED_H
#define MESH_TEXTURED_H

#define _CRT_SECURE_NO_WARNINGS

#include "Mesh.h"

#include <glm/glm.hpp>
#include <vector>

class MeshTextured: public Mesh
{
	std::vector<glm::vec2> textureCoordinates;
	std::vector<int> textureIndices;
	std::vector<GLfloat> verticesTextures;
public:
	MeshTextured();
	GLuint textureId;
	virtual void loadFromFile(const char* fileName);
	virtual void setup();
};

#endif MESH_TEXTURED_H

