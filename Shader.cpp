#include "Shader.h"
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	FILE* f = fopen(vertexShaderPath, "r");
	if (f == NULL)
	{
		throw "vertex shaderfile cant be open";
	}
	fseek(f, 0, SEEK_END);
	int length = ftell(f);
	char* vertexBuffer = (char*)malloc(length);
	if (vertexBuffer == NULL)
	{
		throw "cant allocate memory for string";
	}
	fseek(f, 0, SEEK_SET);
	fread(vertexBuffer, 1, length, f);
	fclose(f);
	f = fopen(fragmentShaderPath, "r");
	if (f == NULL)
	{
		throw "fragment shader cant be open";
	}
	fseek(f, 0, SEEK_END);
	length = ftell(f);
	char* fragmentBuffer = (char*)malloc(length);
	fseek(f, 0, SEEK_SET);
	fread(fragmentBuffer, 1, length, f);
	fclose(f);

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexBuffer, NULL);
	glCompileShader(vertexShader);

	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentBuffer, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	this->programId = shaderProgram;
}

void Shader::use()
{
	glUseProgram(programId);
}
