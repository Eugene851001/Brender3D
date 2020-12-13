#ifndef BALL_H
#define BALL_H

#include <glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "Rectangle3D.h"

struct CollisionInfo
{
	bool isCollision;
	glm::vec3 difference;
//	CollsionInfo(bool isCollision, glm::vec3)
};

class Ball
{
protected:
	float speed;
	float radius;
	GLuint VBO, VAO, EBO;
	int stackCount, sectorCount;
	glm::vec3 color;
public:

	Ball(glm::vec3 position, glm::vec3 direction, float radius, float speed, glm::vec3(color));
	glm::vec3 getPosition();
	void setPosition(glm::vec3 position);
	glm::vec3 getDirection();
	float getRadius();
	void setDirection(glm::vec3 newDirection);
	void move(float time);
	static std::vector<float> getSphereVerticies(float radius, int stackCount, int sectorCount);
	static std::vector<unsigned int> getSphereIndices(int stackCount, int sectorCount);
	GLuint getEBO();
	GLuint getVAO();
	glm::vec3 getColor();
	void setColor(glm::vec3 color);
	int getIndicesSize();
	CollisionInfo collision(rect::Rectangle3D rect);
	glm::vec3 position;
	glm::vec3 direction;
};

#endif

