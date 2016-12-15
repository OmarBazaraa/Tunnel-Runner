#include "FrameTimer.h"

/* Constructor */
FrameTimer::FrameTimer() {
	this->FramesCount = 0;
	this->FPS = 0;
	this->LastTime = 0;
	this->LastFrameTime = 0;
	this->CurrentFrameTime = 0;
	this->ElapsedFramesTime = 0;
}

/* Destructor */
FrameTimer::~FrameTimer() {

}

/* Processes elapsed time between frames and extra calculations */
void FrameTimer::ProcessFrameTime(double time) {
	this->CurrentFrameTime = time;
	this->ElapsedFramesTime = this->CurrentFrameTime - this->LastFrameTime;
	this->LastFrameTime = this->CurrentFrameTime;
	this->FramesCount++;

	if (this->CurrentFrameTime - this->LastTime >= SECOND) {
		this->FPS = this->FramesCount;
		this->FramesCount = 0;
		this->LastTime += SECOND;

		//std::cout << "FPS: " << this->FPS << std::endl;
	}
}