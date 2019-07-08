//StereoGrabber.h is the header file which contains the definition 
//for variables and functions used to obtain images for processing.
#pragma once
#include "stdafx.h"
#include <iostream>
#include "cv.h"
#include "highgui.h"
#include "cvaux.h"
#define WIDTH 352
#define HEIGHT 288
struct StereoGrabber{
	
	void stereoGrabberInitFrames();
	void stereoGrabberGrabFrames();	//grab a frame from the stream
	void stereoGrabberStopCam();
	IplImage* imageLeft;
	IplImage* imageRight;
};
