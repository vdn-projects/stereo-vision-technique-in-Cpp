//StereoFunctions.h contains the definitions for variables 
//and functions used to rectify, correlate, and re-project stereo images.
#pragma once
#include "stdafx.h"

#include <iostream>
#include <sstream>
#include <string>
#include <fstream> 
#include <vector>

#include "BlobResult.h"
#include "cv.h"
#include "highgui.h"
#include "cvaux.h"
#include "StereoGrabber.h"
#include "Interpolation.h"
#include <cxtypes.h>
#include <tchar.h>
using namespace std;

struct StereoFunctions{

	CvMat* _M1;
	CvMat* _M2;
	CvMat* _T;
	CvMat* mx1; 
	CvMat* mx2;
	CvMat* my1; 
	CvMat* my2; 
	CvMat* _Q;			//reprojection matrix
	CvMat* _CamData;

	CvSize imageSize;	//size of input images 
	IplImage	*img1,	//left image
				*img2,	//right image
				*rgb,
				*thres_img,
				*blobs_img,
				*img_detect,
				*real_disparity;
	CvMat	*img1r,		//rectified left image
			*img2r,		//rectified right image
			*disp,		//disparity map
			*vdisp,		//scaled disparity map for viewing
			*pair,
			*depthM;


	void stereoInit(StereoGrabber*);
	void stereoCorrelation(StereoGrabber*);	
	void CorrelationBMState();
	void stereoSavePointCloud();
	void stereoCalibration(StereoGrabber*);
	void stereoDetect();
	double ComputeDis(CvMat*,int row, int col);
	double reprojectionVars[6];
	

	int stereoPreFilterSize, 
		stereoPreFilterCap,  
		stereoDispWindowSize,
		stereoNumDisparities,
		stereoDispTextureThreshold,
		stereoDispUniquenessRatio,
		stereoSavePointCloudValue, 
		stereoSaveOriginal;


	int fileNO, threshold, blobArea;

	
	
};