#include "Ball.h"
#include <tuple>
#define PI 3.1415f

Ball::Ball(glm::vec3 position, glm::vec3 direction, float radius, float speed, glm::vec3 color)
{
	this->position = position;
	this->direction = direction;
	this->radius = radius;
	this->speed = speed;
	VBO = VAO = EBO = -1; 
	stackCount = sectorCount = 20;
	this->color = color;
}

glm::vec3 Ball::getPosition() 
{
	return position;
}

glm::vec3 Ball::getColor()
{
	return color;
}

void Ball::setColor(glm::vec3 color)
{
	this->color = color;
}


void Ball::move(float time)
{
	position += direction * speed * time;
}

void Ball::setDirection(glm::vec3 newDirection)
{
	direction = newDirection;
}

glm::vec3 Ball::getDirection()
{
	return direction;
}

float Ball::getRadius()
{
	return radius;
}

void Ball::setPosition(glm::vec3 position)
{
	this->position = position;
}

std::vector<float> Ball::getSphereVerticies(float radius, int stackCount, int sectorCount)
{
	std::vector<float> verticies;
	float stackStep = PI / stackCount;
	float sectorStep = 2 * PI / sectorCount;
	float x, y, z;
	float stackAngle, sectorAngle;
	//stack - широта
	//sector - долгота
	for (int i = 0; i < stackCount; i++)
	{
		stackAngle = PI / 2 - i * stackStep;
		z = radius * sin(stackAngle);
		for (int j = 0; j < sectorCount; j++)
		{
			sectorAngle = j * sectorStep;

			x = radius * cos(stackAngle) * cos(sectorAngle);
			y = radius * cos(stackAngle) * sin(sectorAngle);

			verticies.push_back(x);
			verticies.push_back(y);
			verticies.push_back(z);
		}
	}
	return verticies;
}

std::vector<unsigned int> Ball::getSphereIndices(int stackCount, int sectorCount)
{
	std::vector<unsigned int> indices;
	int k1, k2;
	for (int i = 0; i < stackCount; i++)
	{
		k1 = i * (sectorCount + 1);
		k2 = k1 + sectorCount + 1;
		for (int j = 0; j < sectorCount; j++, k1++, k2++)
		{
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}
			if (i != (stackCount - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}
	return indices;
}

GLuint Ball::getVAO()
{
	if (VAO != -1)
		return VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	std::vector<float> vertices = getSphereVerticies(radius, stackCount, sectorCount);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);

	std::vector<unsigned int> sphereIndices = Ball::getSphereIndices(stackCount, sectorCount);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), &sphereIndices[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	glBindVertexArray(0);

}

GLuint Ball::getEBO()
{
	if (EBO != -1)
		return EBO;
	getVAO();
	return EBO;
}

int Ball::getIndicesSize()
{
	return stackCount * sectorCount * 6 - 6;
}

CollisionInfo Ball::collision(rect::Rectangle3D rect)
{
	glm::vec3 halfRectSize = glm::vec3(rect.length / 2, rect.height / 2, rect.width / 2);
	glm::vec3 rectCenter = glm::vec3(rect.position.x + rect.length / 2, rect.position.y 
		- rect.height / 2, rect.position.z - rect.width / 2);
	glm::vec3 difference = position - rectCenter;
	glm::vec3 clamp = glm::clamp(difference, -halfRectSize, halfRectSize);
	glm::vec3 closestPoint = rectCenter + clamp;
	difference = position - closestPoint;

	CollisionInfo result;
	if (glm::length(difference) < radius)
		result = { true, difference };
	else
		result = {false, glm::vec3(0, 0, 0)};
	return result;
}
