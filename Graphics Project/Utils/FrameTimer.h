#pragma once

// STL Includes
#include <iostream>
using namespace std;

// Constants
const double SECOND = 1.0;


/*
	Defines some variables needed for frame time calculations
*/
class FrameTimer
{
public:
	// Time
	int FramesCount;			// FPS counter
	double LastTime;
	double LastFrameTime;  		// Time of last frame
	double CurrentFrameTime;	// Start time of the current frame
	double ElapsedFramesTime;	// Elapsed time between current frame and last frame
	
	/* Constructor */
	FrameTimer();

	/* Destructor */
	~FrameTimer();

	/* Processes elapsed time between frames and extra calculations */
	void ProcessFrameTime(double time);
};