#ifndef CAMERA_H
#define CAMERA_H

/// <summary>
/// A struct containing important camera components in a 
/// compound format for the Camera.
/// </summary>
struct Transform
{
public:
	vec3 position = vec3(0.0f);
	vec3 fwd = vec3(0.0f);
	vec3 up = vec3(0.0f);
	vec3 right = vec3(0.0f);

	Transform(){}

	Transform(vec3 startPos, vec3 startFwd)
	{
		position = startPos;
		fwd = startFwd;
	}
};

class Camera
{
public:
	//Camera transform
	Transform transform;

	//Camera parameters
	float movementSpeed;
	float rotationSpeed;
	float fov;
	float pitch;
	float yaw;

	Camera(vec3 startingPos, vec3 startingFwd)
	{
		transform = Transform(startingPos, startingFwd);

		pitch = 0.0f;
		yaw = -90.0f;

		movementSpeed = 2.0f;
		rotationSpeed = 0.2f;

		fov = 45.0f; //Fov = zoom;

		updateCameraVectors();
	}

	mat4 getViewMatrix() {
		return lookAt(transform.position, transform.position + transform.fwd, transform.up);
	}
private:
	void updateCameraVectors() {
		//TODO: Update camera vectors
	}
};
#endif