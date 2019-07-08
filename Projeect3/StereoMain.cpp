// Project.cpp : Defines the entry point for the console application.
//The StereoMain.cpp file defines the entry point for the stereo vision system tools.  
//From the main, the stereo functions for rectification, correlation, and re-projection are called.

#include "stdafx.h"
#pragma warning( disable: 4996 )

#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream> 
#include <ctype.h>
#include <stdlib.h>
//#include <time.h>
#include <windows.h>

#include "cv.h"
#include "highgui.h"
#include "cvaux.h"
#include "StereoGrabber.h"
#include "StereoFunctions.h"

using std::string; 
using std::vector; 
using namespace std;

StereoGrabber* grab = new StereoGrabber();
StereoFunctions* stereoFunc = new StereoFunctions();

//CV_EXTERN_C_FUNCPTR(void(*CvMouseCallback)(int event, int x, int y, int flags, void* param) );
//static void cvSetMouseCallback(const char* window_name, CvMouseCallback* on_mouse, void* param=NULL);
void mouseHandler(int event, int x, int y, int flags, void* param);	//handling mouse events

//functions for handling stereo correlation tools events 
void onWindowBarSlide(int pos);
void onTextureBarSlide(int pos);
void onUniquenessBarSlide(int pos);
void onPreFilterSizeBarSlide(int pos); 
void onPreFilterCapBarSlide(int pos); 
void onSaveDataSlide(int pos);
void onSaveOriginalsSlide(int pos);
void stereoCreateCorrelationControls();
void loadCorrelationParams();
void loadCorrelationParams();

bool fexists(const char *filename);


int main()
{
	if(CALIBRATION) stereoFunc->stereoCalibration(grab);

	loadCorrelationParams();
	grab->stereoGrabberInitFrames();
	grab->stereoGrabberGrabFrames();
	stereoFunc->stereoInit(grab);

	while(1)
	{
		if(ANALYSIS_MODE)	stereoCreateCorrelationControls();
		
		stereoFunc->stereoCorrelation(grab);
		cvSetMouseCallback( "Detect Object", mouseHandler, NULL );
		
		//stereoFunc->stereoSavePointCloud();

		grab->stereoGrabberGrabFrames();

		if( cvWaitKey(10) == 27 ) break;
	}
	
	grab->stereoGrabberStopCam();
	
	return 0;
}


void loadCorrelationParams()
{
	ifstream params;
	params.open("CorrelationParams.txt");
	if (!params) {
		cerr << "Unable to open file Correlation Params file!";
		exit(1);	// call system to stop
	}
	else{
		string temp; 
		int value; 
		while(params>>temp)
		{
			params>>value;
			stereoFunc->stereoDispWindowSize = value;
			params>>temp;
			params>>value;
			stereoFunc->stereoDispTextureThreshold = value;
			params>>temp;
			params>>value;
			stereoFunc->stereoDispUniquenessRatio = value;
			params>>temp;
			params>>value;
			stereoFunc->stereoNumDisparities = value;
			params>>temp;
			params>>value;
			stereoFunc->threshold = value;
			params>>temp;
			params>>value;
			stereoFunc->blobArea = value;
		}
		stereoFunc->stereoPreFilterSize = 63;
		stereoFunc->stereoPreFilterCap = 63; 
		stereoFunc->stereoSavePointCloudValue = 0; 
//		stereoFunc->stereoSaveOriginal = 0;

		params.close();
	}
}
bool fexists(const char *filename)
{
	ifstream ifile(filename);
	return ifile;
}

void onWindowBarSlide(int pos)
{
	stereoFunc->stereoDispWindowSize = cvGetTrackbarPos("SADSize", "Stereo Controls");
	if(stereoFunc->stereoDispWindowSize < 5)
		{	stereoFunc->stereoDispWindowSize = 5;
			stereoFunc->stereoCorrelation(grab);
		}	
	else if ( stereoFunc->stereoDispWindowSize%2 == 0) 	  
		{
			stereoFunc->stereoDispWindowSize += 1;
			stereoFunc->stereoCorrelation(grab);
		}
	else stereoFunc->stereoCorrelation(grab);
}

void onTextureBarSlide(int pos)
{
	stereoFunc->stereoDispTextureThreshold = cvGetTrackbarPos("Texture th", "Stereo Controls");
	if(stereoFunc->stereoDispTextureThreshold) 
		stereoFunc->stereoCorrelation(grab);
}

void onUniquenessBarSlide(int pos)
{
	stereoFunc->stereoDispUniquenessRatio = cvGetTrackbarPos("Uniqueness", "Stereo Controls");
	if(stereoFunc->stereoDispUniquenessRatio>=0)
		stereoFunc->stereoCorrelation(grab);
}
/*void onPreFilterSizeBarSlide(int pos)
{
	stereoFunc->stereoPreFilterSize = cvGetTrackbarPos("PrFil.Size", "Stereo Controls");
	if(stereoFunc->stereoPreFilterSize>=5)
		if(stereoFunc->stereoPreFilterSize%2!=0)
				stereoFunc->stereoCorrelation(grab);
		else {
				++(stereoFunc->stereoPreFilterSize);
				stereoFunc->stereoCorrelation(grab);}
	else {
				stereoFunc->stereoPreFilterSize = 5;
				stereoFunc->stereoCorrelation(grab);}
		
} 

void onPreFilterCapBarSlide(int pos)
{
	stereoFunc->stereoPreFilterCap = cvGetTrackbarPos("PrFil.Cap", "Stereo Controls");
	if(stereoFunc->stereoPreFilterCap == 0) 
		{	stereoFunc->stereoPreFilterCap = 1;
			stereoFunc->stereoCorrelation(grab);
		}
	else if( stereoFunc->stereoPreFilterCap > 63)		
		{	stereoFunc->stereoPreFilterCap = 63;
			stereoFunc->stereoCorrelation(grab);
		}
	else 	stereoFunc->stereoCorrelation(grab);
}*/

void onNumDisparitiesSlide(int pos)
{
	stereoFunc->stereoNumDisparities = cvGetTrackbarPos("Num.Disp", "Stereo Controls");
	while(stereoFunc->stereoNumDisparities%16!=0 || stereoFunc->stereoNumDisparities==0)
		stereoFunc->stereoNumDisparities++;

	stereoFunc->stereoCorrelation(grab);
}
void onSaveDataSlide(int pos)
{
	cvSaveImage("Distance//Img1r.jpeg",stereoFunc->img1r);
	cvSaveImage("Distance//DisparityMap.jpeg",stereoFunc->vdisp);
	stereoFunc->stereoSavePointCloud();  
} 

void onSaveOriginalsSlide(int pos)
{
	
	cvSaveImage("Left.jpeg", stereoFunc->img1);
	cvSaveImage("Right.jpeg", stereoFunc->img2);

}

void stereoCreateCorrelationControls()
{
	cvNamedWindow( "Stereo Controls", 0);
	cvResizeWindow("Stereo Controls", 350, 600);
	cvCreateTrackbar("SADSize", "Stereo Controls", &stereoFunc->stereoDispWindowSize,255, onWindowBarSlide);
	cvCreateTrackbar("Texture th", "Stereo Controls", &stereoFunc->stereoDispTextureThreshold,25, onTextureBarSlide);
	cvCreateTrackbar("Uniqueness", "Stereo Controls", &stereoFunc->stereoDispUniquenessRatio,25, onUniquenessBarSlide);
//	cvCreateTrackbar("PrFil.Size", "Stereo Controls", &stereoFunc->stereoPreFilterSize,101, onPreFilterSizeBarSlide);
//	cvCreateTrackbar("PrFil.Cap", "Stereo Controls", &stereoFunc->stereoPreFilterCap,63, onPreFilterCapBarSlide);
	cvCreateTrackbar("Num.Disp", "Stereo Controls", &stereoFunc->stereoNumDisparities,640, onNumDisparitiesSlide);
	cvCreateTrackbar("Save Dist", "Stereo Controls", &stereoFunc->stereoSavePointCloudValue,1, onSaveDataSlide);
//	cvCreateTrackbar("Save.Img", "Stereo Controls", &stereoFunc->stereoSaveOriginal,1, onSaveOriginalsSlide);

	cvCreateTrackbar("Threshold", "Stereo Controls", &stereoFunc->threshold,300,0);
	cvCreateTrackbar("Area", "Stereo Controls", &stereoFunc->blobArea,5000,0);
}

void mouseHandler(int event, int x, int y, int flags, void *param){

	switch(event){
	case CV_EVENT_LBUTTONDOWN:
	//l = cvGet2D(stereoFunc->depthM, x, y);
	printf("Distance to this object is: %f cm \n",(float)cvGet2D(stereoFunc->depthM, x, y).val[0]);
	break;
	}
}