#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up)
{
	this->position = position;
	this->front = front;
	this->up = up;
	pitch = 0;
	yaw = -90;
	speed = 1;
}

void Camera::moveFront(float time)
{
	position += front * speed * time;
}

void Camera::moveBack(float time)
{
	position -= front * speed * time;
}

void Camera::moveRight(float time)
{
	position += glm::normalize(glm::cross(front, up)) * speed * time;
}

void Camera::moveLeft(float time)
{
	position -= glm::normalize(glm::cross(front, up)) * speed * time;
}

void Camera::rotate(float deltaX, float deltaY)
{
	yaw += deltaX;
	pitch += deltaY;



	front.x = glm::cos(glm::radians(pitch)) * glm::cos(glm::radians(yaw));
	front.y = glm::sin(glm::radians(pitch));
	front.z = glm::cos(glm::radians(pitch)) * glm::sin(glm::radians(yaw));
	front = glm::normalize(front);
}


glm::mat4 Camera::getLookAt()
{
	return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getLookAtForSkybox()
{
	return glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0) + front, up);
}

glm::vec3 Camera::getPosition()
{
	return position;
}

glm::vec3 Camera::getDirection()
{
	return front;
}