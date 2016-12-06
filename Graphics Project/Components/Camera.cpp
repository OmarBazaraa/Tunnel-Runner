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

	// Camera options
	this->mMovementSpeed = SPEED;
	this->mMouseSensitivity = MOUSE_SENSITIVTY;
	this->mFOV = MAX_FOV;
	this->mAspectRatio = aspect;
	this->mNearPlane = NEAR_PLANE;
	this->mFarPlane = FAR_PLANE;

	// Cursor variables
	this->mCursorLastX = 0;
	this->mCursorLastY = 0;
	this->mFirstFrame = true;

	this->UpdateCameraVectors();
}

/* Destructor */
Camera::~Camera() {
	
}

/* Returns camera position in world's coordinates */
glm::vec3 Camera::GetPosition() const {
	return this->mPosition;
}

/* Returns camera direction in world's coordinates */
glm::vec3 Camera::GetFront() const {
	return this->mFront;
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

/* Moves the camera in a certain direction */
void Camera::Move(CameraDirection direction, double deltaTime) {
	float velocity = this->mMovementSpeed * deltaTime;

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
void Camera::ChangeDirection(double xpos, double ypos, double deltaTime, bool constrainPitch) {
	if (this->mFirstFrame) {
		this->mFirstFrame = false;
		this->mCursorLastX = xpos;
		this->mCursorLastY = ypos;
	}

	double xoffset = (xpos - this->mCursorLastX) * deltaTime;
	double yoffset = (this->mCursorLastY - ypos) * deltaTime;  // Reversed since y-coordinates go from bottom to left
	
	this->mCursorLastX = xpos;
	this->mCursorLastY = ypos;

	this->mHorAngle += xoffset * this->mMouseSensitivity;
	this->mVerAngle += yoffset * this->mMouseSensitivity;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
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
