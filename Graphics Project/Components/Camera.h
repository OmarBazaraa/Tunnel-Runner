#pragma once

// STL Includes
#include <vector>

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Include
#include "Shader.h"


/*
	Defines several possible options for camera movement.
	Used as abstraction to stay away from window-system specific input methods
*/
enum CameraDirection {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const glm::vec3 POSITION(0.0f, 0.0f, 0.0f);
const glm::vec3 FRONT(0.0f, 0.0f, -1.0f);
const glm::vec3 WORLD_UP(0.0f, 1.0f, 0.0f);
const double YAW = -90.0f;
const double PITCH = 0.0f;
const double MAX_PITCH = 89.0f;
const double MIN_PITCH = -89.0f;
const double SPEED = 5.0f;
const double MOUSE_SENSITIVTY = 5.0f;
const double MAX_FOV = 45.0f;
const double MIN_FOV = 1.0f;
const double ASPECT_RATIO = 1.3333f;
const double NEAR_PLANE = 0.1f;
const double FAR_PLANE = 100.0f;

/*
	An abstract camera class that processes input and calculates the corresponding
	Eular Angles, Vectors and Matrices for transformation of models
*/
class Camera
{
private:
	// Camera Attributes
	glm::vec3 mPosition;
	glm::vec3 mFront;
	glm::vec3 mUp;
	glm::vec3 mRight;
	glm::vec3 mWorldUp;
	// Eular Angles
	double mHorAngle;	// Rotation angle around y-axis
	double mVerAngle;	// Rotation angle around x-axis
	// Camera options
	double mMovementSpeed;
	double mMouseSensitivity;
	double mFOV;
	double mAspectRatio;
	double mNearPlane;
	double mFarPlane;
	// Cursor variables
	double mCursorLastX;
	double mCursorLastY;
	bool mFirstFrame;

public:
	/* Constructs the camera and setup its vectors */
	Camera(glm::vec3 position = POSITION, double aspect = ASPECT_RATIO);

	/* Destructor */
	~Camera();

	/* Returns camera position in world's coordinates */
	glm::vec3 GetPosition() const;

	/* Returns camera direction in world's coordinates */
	glm::vec3 GetFront() const;

	/* Returns the view matrix calculated using Eular Angles and the LookAt Matrix */
	glm::mat4 GetViewMatrix() const;

	/* Returns the projection matrix */
	glm::mat4 GetProjectionMatrix() const;

	/* Applies camera view by sending the related matrices to the shader */
	void ApplyEffects(const Shader& shader);

	/* Moves the camera in a certain direction */
	void Move(CameraDirection direction, double deltaTime);

	/* Changes camera direction by certain offset in X and Y */
	void ChangeDirection(double xpos, double ypos, double deltaTime, bool constrainPitch = true);

	/* Changes the zoom effect of the camera by a certain offset */
	void Zoom(double offset);

private:
	/* Calculates the front vector from the camera's (updated) Eular Angles */
	void UpdateCameraVectors();
};
