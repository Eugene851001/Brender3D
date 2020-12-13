#ifndef RECTANGLE3D_H
#define RECTANGLE3D_H

#include <glm/glm.hpp>

#include <glew.h>

namespace rect 
{

	static float cubeVertices[] = { -0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,
							0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,
							0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,
							0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,
							-0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,
							-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,

							-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,
							0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,
							0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,
							0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,
							-0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,
							-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,

							-0.5f, 0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,
							-0.5f, 0.5f, -0.5f,		-1.0f, 0.0f, 0.0f,
							-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,
							-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,
							-0.5f, -0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,
							-0.5f, 0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,

							0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 0.0f,
							0.5f, 0.5f, -0.5f,		1.0f, 0.0f, 0.0f,
							0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,
							0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,
							0.5f, -0.5f, 0.5f,		1.0f, 0.0f, 0.0f,
							0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 0.0f,

							-0.5f, -0.5f, -0.5f,	 0.0f, -1.0f, 0.0f,
							0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,
							0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,
							0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,
							-0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,
							-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,

							-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,
							0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,
							0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,
							0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,
							-0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,
							-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,
	};

	enum RectangleType {RECT_SIMPLE, RECT_TEXTURED};

	class Rectangle3D
	{
	protected:
		GLuint VAO, VBO;
		GLfloat* vertices;
		RectangleType rectType;
	public:
		glm::vec3 position;//left high front
		float length, height, width;
		Rectangle3D(glm::vec3 point, float length, float height, float width);
		~Rectangle3D();
		virtual GLfloat* getVertexArray(int& size);
		virtual GLuint getVAO();
		bool isCollision(Rectangle3D rect);
		RectangleType getRectangleType();
	};
}

#endif