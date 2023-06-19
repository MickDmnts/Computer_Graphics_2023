#ifndef CAMERA_H
#define CAMERA_H

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Global.h"

enum CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera
{
public:
	//Camera transform
	glm::vec3 position;
	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;

	//Camera parameters
	float movementSpeed;
	float rotationSpeed;
	float fov;
	float pitch;
	float yaw;

	Camera()
	{
		position = glm::vec3(0.0f);
		forward = glm::vec3(0.0f);
		up = glm::vec3(0.0f);
		right = glm::vec3(0.0f);

		movementSpeed = 0.0f;
		rotationSpeed = 0.0f;
		fov = 0.0f;
		pitch = 0.0f;
		yaw = 0.0f;
	}

	Camera(glm::vec3 startingPosition, glm::vec3 startingForward)
	{
		position = startingPosition;
		forward = startingForward;

		pitch = 0.0f;
		yaw = -90.0f;

		movementSpeed = 2.0f;
		rotationSpeed = 1.0f;

		fov = 45.0f; //fov = zoom

		UpdateCameraVectors();
	}

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(position, position + forward, up);
	}

	void HandleKeyboard(CameraMovement direction, float deltaTime)
	{
		float finalSpeed = movementSpeed * deltaTime;

		switch (direction)
		{
			case FORWARD:
				position += forward * finalSpeed;
				break;

			case BACKWARD:
				position -= forward * finalSpeed;
				break;

			case LEFT:
				position -= right * finalSpeed;
				break;

			case RIGHT:
				position += right * finalSpeed;
				break;

			case UP:
				position += up * finalSpeed;
				break;

			case DOWN:
				position -= up * finalSpeed;
				break;
		}
	}

	void HandleMouseMovement(float xOffset, float yOffset, float deltaTime)
	{
		float finalSpeed = rotationSpeed * deltaTime;

		yaw += xOffset * finalSpeed;
		pitch -= yOffset * finalSpeed;

		if (pitch < -90.0f)
			pitch = -90.0f;
		if (pitch > 90.0f)
			pitch = 90.0f;

		UpdateCameraVectors();
	}

	void HandleMouseScroll(float offset)
	{
		fov -= offset;

		if (fov < 1.0f)
			fov = 1.0f;
		if (fov > 90.0f)
			fov = 90.0f;
	}

private:
	void UpdateCameraVectors()
	{
		glm::vec3 direction;
		direction.x = glm::cos(glm::radians(yaw));
		direction.y = glm::sin(glm::radians(pitch));
		direction.z = glm::sin(glm::radians(yaw));

		forward = glm::normalize(direction);
		right = glm::normalize(glm::cross(forward, WORLD_UP)); // works because we do not have ROLL
		up = glm::normalize(glm::cross(right, forward));
	}
};

#endif