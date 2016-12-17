#include "Camera.h"

/* Constructs the camera and setup its vectors */
Camera::Camera(glm::vec3 position, double aspect) {
	// Camera Attributes
	this->mPosition = position;
	this->mFront = FRONT;
	this->mWorldUp = WORLD_UP;

	// Eular Angles
	this->mHorAngle = YAW;
	this->mVerAngle = PITCH;

	// Animation variables
	// Horizontal Move
	this->mIsMovingHorizontalStep = false;
	this->mIsMovingForwardStep = false;
	this->mMoveSpeed = MOVE_SPEED_INIT;
	this->mMoveAcceleration = MOVE_ACCELERATION_INIT;
	// Jump
	this->mIsJumping = false;
	this->mJumpVelocity = JUMP_SPEED;
	this->mJumpAcceleration = JUMP_ACCELERATION;
	this->mGroundPosition = position.y;
	// Look around
	this->mMouseSensitivity = MOUSE_SENSITIVTY;

	// Camera options
	this->mFOV = MAX_FOV;
	this->mAspectRatio = aspect;
	this->mNearPlane = NEAR_PLANE;
	this->mFarPlane = FAR_PLANE;

	// Cursor variables
	this->mCursorLastX = 0;
	this->mCursorLastY = 0;
	this->mFirstFrame = true;

	// Calculate camera vectors
	this->UpdateCameraVectors();
}

/* Destructor */
Camera::~Camera() {
	
}

/* Sets the position of the camera in world space */
void Camera::SetPosition(glm::vec3 position) {
	this->mPosition = position;
}

/* Returns camera position in world's coordinates */
glm::vec3 Camera::GetPosition() const {
	return this->mPosition;
}

/* Returns camera direction in world's coordinates */
glm::vec3 Camera::GetFront() const {
	return this->mFront;
}

/* Sets the position of the ground/gravity, needed to apply falling effect */
void Camera::SetGravityPosition(double ypos) {
	if (this->mGroundPosition > ypos && !this->mIsJumping) {
		this->mJumpVelocity = 0.0f;
		this->mIsJumping = true;
	}

	this->mGroundPosition = ypos;
}

/* Sets the movement speed of the camera */
void Camera::SetMoveSpeed(double speed) {
	this->mMoveSpeed = speed;
}

/* Sets the Camera movement acceleration with a certain value */
void Camera::SetMoveAcceleration(double acceleration) {
	this->mMoveAcceleration = acceleration;
}

/* Increases the camera speed by the given amount */
void Camera::AccelerateSpeed() {
	this->mMoveSpeed += this->mMoveAcceleration;

	if (this->mMoveSpeed > MOVE_SPEED_MAX) {
		this->mMoveSpeed = MOVE_SPEED_MAX;
	}
}

/* Returns whether the camera is moving left */
bool Camera::IsMovingLeft() const {
	return this->mIsMovingHorizontalStep && this->mMoveHorizontalDirection < 0.0f;
}

/* Returns whether the camera is moving right */
bool Camera::IsMovingRight() const {
	return this->mIsMovingHorizontalStep && this->mMoveHorizontalDirection > 0.0f;
}

/* Returns whether the camera is jumping */
bool Camera::IsJumping() const {
	return this->mIsJumping && this->mJumpVelocity > 0.0f;
}

/* Returns whether the camera is falling */
bool Camera::IsFalling() const {
	return this->mIsJumping && this->mJumpVelocity < 0.0f;
}

/* Returns the view matrix calculated using Eular Angles and the LookAt Matrix */
glm::mat4 Camera::GetViewMatrix() const {
	return glm::lookAt(this->mPosition, this->mPosition + this->mFront, this->mUp);
}

/* Returns the projection matrix */
glm::mat4 Camera::GetProjectionMatrix() const {
	return glm::perspective(this->mFOV, this->mAspectRatio, this->mNearPlane, this->mFarPlane);
}

/* Applies camera view by sending the related matrices to the shader */
void Camera::ApplyEffects(const Shader& shader) {
	glUniformMatrix4fv(shader.ViewMatrixLoc, 1, GL_FALSE, glm::value_ptr(this->GetViewMatrix()));
	glUniformMatrix4fv(shader.ProjectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(this->GetProjectionMatrix()));
	glUniform3f(shader.CameraPositionLoc, this->mPosition.x, this->mPosition.y, this->mPosition.z);
}

/* Moves the camera a step in a certain direction */
void Camera::MoveStep(CameraDirection type, double offset) {
	switch (type)
	{
	case FORWARD:
		if (!this->mIsMovingForwardStep) {
			this->mIsMovingForwardStep = true;
			this->mMoveForwardDirection = -1.0;
			this->mMoveForwardOffset = offset;
			this->mMoveForwardDestination = this->mPosition.z - offset;
		}
		break;
	case BACKWARD:
		if (!this->mIsMovingForwardStep) {
			this->mIsMovingForwardStep = true;
			this->mMoveForwardDirection = 1.0;
			this->mMoveForwardOffset = offset;
			this->mMoveForwardDestination = this->mPosition.z + offset;
		}
		break;
	case LEFT:
		if (!this->mIsMovingHorizontalStep) {
			this->mIsMovingHorizontalStep = true;
			this->mMoveHorizontalDirection = -1.0;
			this->mMoveHorizontalOffset = offset;
			this->mMoveHorizontalDestination = this->mPosition.x - offset;
		}
		break;
	case RIGHT:
		if (!this->mIsMovingHorizontalStep) {
			this->mIsMovingHorizontalStep = true;
			this->mMoveHorizontalDirection = 1.0;
			this->mMoveHorizontalOffset = offset;
			this->mMoveHorizontalDestination = this->mPosition.x + offset;
		}
		break;
	}
}

/* Starts jumping animation */
void Camera::Jump(double offset) {
	// TODO: Force the camera to jump with a certain offset
	if (!this->mIsJumping) {
		this->mIsJumping = true;
		this->mJumpVelocity = std::sqrt(2 * JUMP_ACCELERATION * offset);
	}
}

/* Updates the camera to apply the animation effects */
void Camera::Update(double deltaTime) {
	// Horizontal move effect
	if (this->mIsMovingHorizontalStep) {
		float velocity =  this->mMoveSpeed * deltaTime;

		this->mPosition.x += velocity * this->mMoveHorizontalDirection;
		this->mMoveHorizontalOffset -= velocity;

		if (this->mMoveHorizontalOffset <= 0.0f) {
			this->mPosition.x = this->mMoveHorizontalDestination;
			this->mIsMovingHorizontalStep = false;
		}
	}

	// Forward move effect
	if (this->mIsMovingForwardStep) {
		float velocity = this->mMoveSpeed * deltaTime;

		this->mPosition.z += velocity * this->mMoveForwardDirection;
		this->mMoveForwardOffset -= velocity;

		if (this->mMoveForwardOffset <= 0.0f) {
			this->mPosition.z = this->mMoveForwardDestination;
			this->mIsMovingForwardStep = false;
		}
	}

	// Jump and falling effects
	if (this->mIsJumping) {
		float velocity = this->mJumpVelocity * deltaTime;

		this->mJumpVelocity -= this->mJumpAcceleration * deltaTime;

		this->mPosition.y += velocity;

		if (this->mPosition.y <= this->mGroundPosition) {
			this->mPosition.y = this->mGroundPosition;
			this->mIsJumping = false;
		}
	}
}

/* Moves the camera in a certain direction */
void Camera::Move(CameraDirection direction, double deltaTime) {
	float velocity = this->mMoveSpeed * deltaTime;

	if (direction == FORWARD)
		this->mPosition += this->mFront * velocity;
	if (direction == BACKWARD)
		this->mPosition -= this->mFront * velocity;
	if (direction == RIGHT)
		this->mPosition += this->mRight * velocity;
	if (direction == LEFT)
		this->mPosition -= this->mRight * velocity;
}

/* Changes camera direction by certain offset in X and Y */
void Camera::ChangeDirection(double xpos, double ypos, double deltaTime, bool constrainYaw, bool constrainPitch) {
	if (this->mFirstFrame) {
		this->mFirstFrame = false;
		this->mCursorLastX = xpos;
		this->mCursorLastY = ypos;
	}

	double xoffset = (xpos - this->mCursorLastX) * deltaTime;
	double yoffset = (this->mCursorLastY - ypos) * deltaTime;  // Reversed since y-coordinates go from bottom to top
	
	this->mCursorLastX = xpos;
	this->mCursorLastY = ypos;

	this->mHorAngle += xoffset * this->mMouseSensitivity;
	this->mVerAngle += yoffset * this->mMouseSensitivity;

	// Make sure that yaw angle is within range
	if (constrainYaw) {
		if (this->mHorAngle > MAX_YAW)
			this->mHorAngle = MAX_YAW;
		if (this->mHorAngle < MIN_YAW)
			this->mHorAngle = MIN_YAW;
	}

	// Make sure that when pitch angle is out of bounds, screen doesn't get flipped
	if (constrainPitch) {
		if (this->mVerAngle > MAX_PITCH)
			this->mVerAngle = MAX_PITCH;
		if (this->mVerAngle < MIN_PITCH)
			this->mVerAngle = MIN_PITCH;
	}

	// Update Front, Right and Up Vectors using the updated Eular angles
	this->UpdateCameraVectors();
}

/* Changes the zoom effect of the camera by a certain offset */
void Camera::Zoom(double offset) {
	this->mFOV -= offset * mMouseSensitivity;

	if (this->mFOV <= MIN_FOV)
		this->mFOV = MIN_FOV;
	if (this->mFOV >= MAX_FOV)
		this->mFOV = MAX_FOV;
}

/* Stop any running animation */
void Camera::StopAnimation() {
	this->mIsJumping = false;
	this->mIsMovingForwardStep = false;
	this->mIsMovingHorizontalStep = false;
}

/* Calculates the front vector from the camera's (updated) Eular Angles */
void Camera::UpdateCameraVectors() {
	glm::vec3 front;
	front.x = cos(glm::radians(this->mHorAngle)) * cos(glm::radians(this->mVerAngle));
	front.y = sin(glm::radians(this->mVerAngle));
	front.z = sin(glm::radians(this->mHorAngle)) * cos(glm::radians(this->mVerAngle));

	this->mFront = glm::normalize(front);
	this->mRight = glm::normalize(glm::cross(this->mFront, this->mWorldUp));
	this->mUp = glm::normalize(glm::cross(this->mRight, this->mFront));
}