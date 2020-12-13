#ifndef TEXTURED_RECTANGLE_3D
#define TEXTURED_RECTANGLE_3D

#include "Rectangle3D.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glew.h"

class TexturedRectangle3D: public rect::Rectangle3D  
{
	Texture* texture;
public:
	TexturedRectangle3D(glm::vec3 position, float length, float heigth, float width, Texture *texture);// : Rectangle3D(position, length, height, width);
	virtual float* getVertexArray(int &size);
	virtual GLuint getVAO();
	GLuint getTextureId();
};

#endif