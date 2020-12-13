#ifndef MESH_NORMALS_TEXTURES_H 
#define MESH_NORMALS_TEXTURES_H

#include "MeshTextured.h"
#include "MeshNormals.h"

class MeshNormalsTextures: public MeshNormals
{
	std::vector<GLfloat> verticesNormalsTextures;
	std::vector<glm::vec2> textureCoordinates;
	std::vector<int> textureIndices;
	std::vector<GLfloat> verticesTextures;
public:
	MeshNormalsTextures();
	GLuint textureId;
	virtual void loadFromFile(const char* filename);
	virtual void setup();
};

#endif

