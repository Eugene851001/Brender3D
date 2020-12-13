#include "Rectangle3D.h"

namespace rect
{
	Rectangle3D::Rectangle3D(glm::vec3 point, float length, float height, float width)
	{
		rectType = RECT_SIMPLE;
		this->position = point;
		this->height = height;
		this->width = width;
		this->length = length;
		this->vertices = NULL;
		VAO = -1;
		VBO = -1;
	}

	float* Rectangle3D::getVertexArray(int& size)
	{
		size = sizeof(cubeVertices);
		if (vertices != NULL)
		{
			return vertices;
		}
		int amount = size / sizeof(float);
		vertices = new float[amount];
		for (int i = 0; i < amount; i++)
		{
			vertices[i] = cubeVertices[i];
		}
		for (int i = 0; i < 6 * 6; i++)
		{
			if (vertices[6 * i] == -0.5)
				vertices[6 * i] = 0;
			else
				vertices[6 * i] = length;

			if (vertices[6 * i + 1] == -0.5)
				vertices[6 * i + 1] = -height;
			else
				vertices[6 * i + 1] = 0;

			if (vertices[6 * i + 2] == -0.5)
				vertices[6 * i + 2] = 0 - width;
			else
				vertices[6 * i + 2] = 0;
		}
		return vertices;
	}

	bool Rectangle3D::isCollision(Rectangle3D rect)
	{
		bool result = rect.position.x < position.x + length && rect.position.x + rect.length > position.x
			&& rect.position.y > position.y - height && rect.position.y - rect.height < position.y
			&& rect.position.z > position.z - width && rect.position.z - rect.width < position.z;

		return result;
	}

	GLuint Rectangle3D::getVAO() 
	{
		if (VAO != -1)
			return VAO;
		GLuint VBO, VAO;
		int size;
		getVertexArray(size);
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0); 

		glBindVertexArray(0); 
		this->VAO = VAO;
		this->VBO = VBO;
		return VAO;
	}

	Rectangle3D::~Rectangle3D()
	{
	/*	if(vertices != NULL)
			free(vertices);
		if(VAO != -1)
			glDeleteVertexArrays(1, &VAO);
		if(VBO != -1)
		glDeleteBuffers(1, &VBO);*/
	}

	RectangleType Rectangle3D::getRectangleType()
	{
		return rectType;
	}
}