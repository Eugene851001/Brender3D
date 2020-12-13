#ifndef SHADER_H
#define SHADER_H

#include <glew.h>

class Shader
{
public:
	GLuint programId;
	Shader(const char *vertexShaderPath, const char *fragmentShaderPath);
	void use();
};

#endif
