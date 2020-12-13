#ifndef MESH_NORMALS_H
#define MESH_NORMALS_H

#define _CRT_SECURE_NO_WARNINGS

#include <glm/glm.hpp>
#include <glew.h>
#include <vector>

#include "Mesh.h"

class MeshNormals: public Mesh
{
public:
	MeshNormals();
	std::vector<glm::vec3> normals;
	std::vector<int> normalsIndices;
	std::vector<glm::vec3> verticesNormals;
	virtual void loadFromFile(const char* fileName);
	virtual void setup();
};

#endif MESH_NORMALS_H

