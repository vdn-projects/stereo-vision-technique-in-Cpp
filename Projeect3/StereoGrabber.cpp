//StereoGrabber.cpp contains the algorithms for obtaining images for 
//processing from various cameras, including networked cameras and fire wire cameras
#include "stdafx.h"
#include "StereoGrabber.h"

CvCapture *capture1= NULL, *capture2= NULL; //capture1 <-> left, capture2 <-> right

void StereoGrabber::stereoGrabberInitFrames()
		{
		capture1= cvCaptureFromCAM(2);
		assert(capture1!=NULL); 
		cvWaitKey(100);
		capture2= cvCaptureFromCAM(1);
		assert(capture2!=NULL);
		
	//assure capture size is correct...
		cvSetCaptureProperty(capture1,CV_CAP_PROP_FRAME_WIDTH,WIDTH);
		cvSetCaptureProperty(capture1,CV_CAP_PROP_FRAME_HEIGHT,HEIGHT);
		cvSetCaptureProperty(capture2,CV_CAP_PROP_FRAME_WIDTH,WIDTH);
		cvSetCaptureProperty(capture2,CV_CAP_PROP_FRAME_HEIGHT,HEIGHT);

		}

void StereoGrabber::stereoGrabberGrabFrames()
		{
		imageLeft = cvQueryFrame(capture1);
		imageRight = cvQueryFrame(capture2);
		}
		
void StereoGrabber::stereoGrabberStopCam()	
		{
		cvReleaseCapture( &capture1 ); 
		cvReleaseCapture( &capture2 );
		}