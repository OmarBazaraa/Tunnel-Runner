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
	RIGHT,
};


// Default camera values
const glm::vec3 POSITION(0.0f, 0.0f, 0.0f);
const glm::vec3 FRONT(0.0f, 0.0f, -1.0f);
const glm::vec3 WORLD_UP(0.0f, 1.0f, 0.0f);
// Eular angles
const double YAW = -90.0f;
const double PITCH = 0.0f;
const double MAX_YAW = YAW + 45.0f;
const double MIN_YAW = YAW - 45.0f;
const double MAX_PITCH = 45.0f;
const double MIN_PITCH = -MAX_PITCH;
// Animation constants
const double MOVE_SPEED_INIT = 4.0f;
const double MOVE_SPEED_MAX = 8.0f;
const double MOVE_ACCELERATION_INIT = 0.01;
const double JUMP_SPEED = 4.0f;
const double JUMP_ACCELERATION = 12.0f;			// If you want to jump with a certain height use this EQN => a = (MOVE_SPEED)^2 / (2 * Height)
const double MOUSE_SENSITIVTY = 5.0f;
// Camera options
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
	// Camera attributes
	glm::vec3 mPosition;
	glm::vec3 mFront;
	glm::vec3 mUp;
	glm::vec3 mRight;
	glm::vec3 mWorldUp;

	// Eular angles
	double mHorAngle;	// Rotation angle around y-axis
	double mVerAngle;	// Rotation angle around x-axis

	// Animation variables
	// Move
	bool mIsMovingHorizontalStep;
	bool mIsMovingForwardStep;
	double mMoveSpeed;
	double mMoveAcceleration;
	double mMoveHorizontalDirection;
	double mMoveForwardDirection;
	double mMoveHorizontalOffset;
	double mMoveForwardOffset;
	double mMoveHorizontalDestination;
	double mMoveForwardDestination;
	// Jump
	bool mIsJumping;
	double mJumpVelocity;
	double mJumpAcceleration;
	double mGroundPosition;
	double mJumpStartHeight;
	// Look around
	double mMouseSensitivity;

	// Camera options
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

	/* Sets the position of the camera by the given vector */
	void SetPosition(glm::vec3 position = POSITION);

	/* Returns camera position in world's coordinates */
	glm::vec3 GetPosition() const;

	/* Returns camera direction in world's coordinates */
	glm::vec3 GetFront() const;

	/* Returns the camera movement speed */
	double Camera::GetCameraSpeed();

	/* Sets the position of the ground/gravity, needed to apply falling effect */
	void SetGravityPosition(double ypos);

	/* Sets the Camera movement speed with a certain value */
	void SetMoveSpeed(double speed);

	/* Sets the Camera movement acceleration with a certain value */
	void SetMoveAcceleration(double acceleration);

	/* Increases the camera speed by the given amount */
	void AccelerateSpeed();

	/* Returns whether the camera is moving left */
	bool IsMovingLeft() const;

	/* Returns whether the camera is moving right */
	bool IsMovingRight() const;

	/* Returns whether the camera is jumping */
	bool IsJumping() const;

	/* Returns height from jumping start position */
	double JumpingOffset() const;

	/* Returns the view matrix calculated using Eular Angles and the LookAt Matrix */
	glm::mat4 GetViewMatrix() const;

	/* Returns the projection matrix */
	glm::mat4 GetProjectionMatrix() const;

	/* Applies camera view by sending the related matrices to the shader */
	void ApplyEffects(const Shader& shader);

	/* Moves the camera a step in a certain direction */
	void MoveStep(CameraDirection type, double offset);

	/* Starts jumping animation */
	void Jump(double offset);

	/* Updates the camera to apply the animation effects */
	void Update(double deltaTime);

	/* Moves the camera in a certain direction */
	void Move(CameraDirection direction, double deltaTime);

	/* Changes camera direction by certain offset in X and Y */
	void ChangeDirection(double xpos, double ypos, double deltaTime, bool constrainYaw = true, bool constrainPitch = true);

	/* Changes the zoom effect of the camera by a certain offset */
	void Zoom(double offset);

	/* Stop any running animation */
	void StopAnimation();

private:
	/* Calculates the front vector from the camera's (updated) Eular Angles */
	void UpdateCameraVectors();
};
