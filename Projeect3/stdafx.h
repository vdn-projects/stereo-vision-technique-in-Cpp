//The stdafx.h file contains the values which define the work flow of the stereo vision system tools.
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once
#include "targetver.h"
#include <string>
#include <stdio.h>
#include <tchar.h>

#define	CV_EVENT_LBUTTONDOWN	1	
#define	CV_EVENT_RBUTTONDOWN	2			

#define CALIBRATION 0	

#define SHOW_DATA 1	//show and save intermediate results from stereo correlation, 
					//set this to 0 when just producing a point cloud
#define SAVE_DATA 0
#define SHOW_DISTANCE 0 //output point cloud in format to read into dylan's or dan's point cloud viewer
#define SAVE_DISTANCE 1	//saves depth information of each pixel in a seperate file

#define ANALYSIS_MODE 1	//set to 1 to enter analysis mode, where intermediate results are shown 
						//and a correlation variable tool can be used
						//extra data can still be saved, but is done manually through a button on the tool
#define PI	3.141592654
// TODO: reference additional headers your program requires here

