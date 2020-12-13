#ifndef SKYBOX_H
#define SKYBOX_H

#include <glew.h>

class Skybox
{
	GLuint VAO, VBO;
	GLuint textureId;
public:
	Skybox(unsigned char* rgbImageData[6]);
	GLuint getVAO();
	GLuint getTextureId();

};

#endif

