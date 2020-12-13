#ifndef TEXTURE_H
#define TEXTURE_H

#include <glew.h>

class Texture
{
	GLuint textureId;
public:
	Texture(const char *imagePath);
	GLuint getId();

};

#endif TEXTURE_H

