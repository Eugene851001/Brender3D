#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	float speed;
	float pitch, yaw;
public:

	Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up);
	void moveFront(float time);
	void moveBack(float time);
	void moveRight(float time);
	void moveLeft(float time);
	void rotate(float deltaX, float deltaY);
	glm::vec3 getDirection();
	glm::vec3 getPosition();
	glm::mat4 getLookAt();
	glm::mat4 getLookAtForSkybox();
};
#endif

