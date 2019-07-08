#include "stdafx.h"
#include "StereoFunctions.h"
#include "math.h" 
#include <cxtypes.h>
#include <iomanip>

Interpolation* Interpol = new Interpolation();
int fileNO = 0;
IplImage *r_detect, *g_detect, *b_detect, *r_detect_r, *g_detect_r, *b_detect_r ;
int threshold, blobArea;
CvFont font;


void StereoFunctions::stereoInit(StereoGrabber* grabb){
	//read  stereo calib file
	// Information for Calibration File.
	// Calibration File Format.
	// (1) M1: Camera Matrix for camera 1 (left) [3][3] row by row
	// (2) M2: Camera Matrix for camera 2 (right) [3][3] row by row
	cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1, 1, 0, 1.4f, CV_AA);
	
	_M1 = (CvMat *)cvLoad("CalibFile/M1.yml");
	_M2 = (CvMat *)cvLoad("CalibFile/M2.yml");
	_T  = (CvMat *)cvLoad("CalibFile/T.yml");
	mx1 = (CvMat *)cvLoad("CalibFile/mx1.yml");
	my1 = (CvMat *)cvLoad("CalibFile/my1.yml");
	mx2 = (CvMat *)cvLoad("CalibFile/mx2.yml");
	my2 = (CvMat *)cvLoad("CalibFile/my2.yml");
	//_Q = (CvMat *)cvLoad("CalibFile/Q.yml");
	_CamData = (CvMat *)cvLoad("CalibFile/CamData.yml");

	//0: fx(pixel), 1: fy(pixel), 2: B (baseline), 3: f(mm), 4: sensor element size, 5: baseline in mm
		reprojectionVars[0] = cvmGet(_M1,0,0);
		reprojectionVars[1] = cvmGet(_M1,0,0);
		reprojectionVars[2] = (-1)*cvmGet(_T,0,0);
		reprojectionVars[3] = cvmGet(_CamData, 0, 0);
		reprojectionVars[4] = cvmGet(_CamData, 0, 1);
		reprojectionVars[5] = cvmGet(_CamData, 0, 2);

		

	//Loading images
		img1 = cvCreateImage(cvSize(grabb->imageLeft->width,grabb->imageLeft->height), IPL_DEPTH_8U, 1);		
		img2 = cvCreateImage(cvSize(grabb->imageRight->width,grabb->imageRight->height), IPL_DEPTH_8U, 1);
		
		imageSize = cvSize(img1->width,img1->height);		
		img1r = cvCreateMat( imageSize.height,imageSize.width, CV_8U );		//rectified left image
		img2r = cvCreateMat( imageSize.height,imageSize.width, CV_8U );		//rectified right image
		disp  = cvCreateMat( imageSize.height,imageSize.width, CV_16S );		//disparity map
		vdisp = cvCreateMat( imageSize.height,imageSize.width, CV_8U );
		depthM = cvCreateMat(imageSize.height, imageSize.width, CV_32F);
				
		
		thres_img = cvCreateImage( imageSize, img1->depth, 1);
		blobs_img = cvCreateImage( imageSize, img1->depth, 3);
		
		img_detect = cvCreateImage( imageSize, grabb->imageLeft->depth, 3);
		r_detect = cvCreateImage(imageSize,8,1);//subpixel
		r_detect_r = cvCreateImage(imageSize,8,1);
		g_detect = cvCreateImage(imageSize,8,1);//subpixel
		g_detect_r = cvCreateImage(imageSize,8,1);
		b_detect = cvCreateImage(imageSize,8,1);//subpixel
		b_detect_r = cvCreateImage(imageSize,8,1);
		
		pair = cvCreateMat( imageSize.height, imageSize.width*2,CV_8UC3 );  

	}	
						
void StereoFunctions::stereoCorrelation(StereoGrabber* grabb)
{	

	//update data for BMState
	CvStereoBMState *BMState = cvCreateStereoBMState(); 
	assert(BMState != 0);
	BMState->preFilterSize = stereoPreFilterSize; 
	BMState->preFilterCap = stereoPreFilterCap;
	BMState->SADWindowSize = stereoDispWindowSize; 
	BMState->minDisparity = 0;
	BMState->numberOfDisparities = stereoNumDisparities; 
	BMState->textureThreshold = stereoDispTextureThreshold; 
	BMState->uniquenessRatio = stereoDispUniquenessRatio;
	BMState->speckleWindowSize = 200;
	BMState->speckleRange = 32;
	BMState->disp12MaxDiff = 2;
	
	cvSplit(grabb->imageLeft,r_detect,g_detect,b_detect, NULL);
	cvRemap( r_detect, r_detect_r, mx1, my1 ); // Undistort image
	cvRemap( g_detect, g_detect_r, mx1, my1 ); // Undistort image
	cvRemap( b_detect, b_detect_r, mx1, my1 ); // Undistort image
	cvMerge( r_detect_r, g_detect_r, b_detect_r, NULL, img_detect);
	
	cvCvtColor(grabb->imageLeft, img1, CV_BGR2GRAY);
	cvCvtColor(grabb->imageRight, img2, CV_BGR2GRAY);
	cvRemap( img1, img1r, mx1, my1 );
	cvRemap( img2, img2r, mx2, my2 );
	cvFindStereoCorrespondenceBM(img1r, img2r, disp, BMState);

	cvNormalize( disp, vdisp, 0, 256, CV_MINMAX );
	
	if(SHOW_DISTANCE) 	stereoSavePointCloud();
	stereoDetect();	
		
	if(SHOW_DATA){

		cvNamedWindow( "Rectified", 1);
		cvNamedWindow( "Disparity Map",1 );
		CvMat part;
		cvGetCols( pair, &part, 0, imageSize.width );

		cvCvtColor( img1r, &part, CV_GRAY2BGR );
		cvGetCols( pair, &part, imageSize.width, imageSize.width*2 );
		cvCvtColor( img2r, &part, CV_GRAY2BGR );
		for( int j = 0; j < imageSize.height; j += 16 )
			cvLine( pair, cvPoint(0,j), cvPoint(imageSize.width*2,j),CV_RGB(0,255,0));

		cvShowImage("Rectified", pair);
		cvShowImage("Disparity Map", vdisp);
		
		if(SAVE_DATA) 
		{
			string dispFile;
			stringstream str, str2, str3;
			string left, right;
			str<<"Disparities/DisparityMap"<<"-"<<fileNO<<".jpg";
			dispFile=str.str();
			cvSaveImage(&dispFile[0],vdisp);
			str2<<"Rectified/Left"<<"-"<<fileNO<<".jpg";
			str3<<"Rectified/Right"<<"-"<<fileNO<<".jpg";
			left=str2.str();
			right=str3.str(); 
			cvSaveImage(&left[0],img1r); 
			cvSaveImage(&right[0],img2r);

			fileNO++;
		}
	}
	cvReleaseStereoBMState(&BMState);
}


	
CvPoint 	rect1, rect2, rect, rect_dist;
CBlobResult blobs;
CBlob 		*currentBlob;
void StereoFunctions::stereoDetect()
{
		
	//	cvSmooth(vdisp,vdisp,CV_GAUSSIAN,3,3);
		cvCmpS(vdisp,threshold,thres_img,CV_CMP_GT);		
		blobs = CBlobResult(thres_img, NULL,0);
		
		/* Remove blobs if it does not cover minimum area specified below */
		blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(),B_LESS,blobArea,4000);

		/* Color the blobs one after the other */
		for (int i = 0; i < blobs.GetNumBlobs(); i++ )
		{
			currentBlob = blobs.GetBlob(i);
			currentBlob->FillBlob( blobs_img, CV_RGB(0,0,255));

			rect1.x = (int)currentBlob->MinX();
			rect1.y = (int)currentBlob->MinY();

			rect2.x = (int)currentBlob->MaxX();
			rect2.y = (int)currentBlob->MaxY();
			
			rect.x = rect1.x ;
			rect.y = (rect1.y + rect2.y)/2 ;
			
			rect_dist.x = rect.x;
			rect_dist.y = rect.y + 15;
			
			stringstream blobstream;
			blobstream << "Object " << i+1;
			string blobNumObj = blobstream.str();
			
			cvPutText(img_detect, &blobNumObj[0], rect, &font, cvScalar(0, 0, 255, 0));
			
			stringstream DistText;
			DistText<< setprecision(3)<<max(max(max((float)ComputeDis(disp,(rect1.y + rect2.y)/2,(rect1.x + rect2.x)/2),
													(float)ComputeDis(disp,rect1.y + 2*(rect2.y - rect1.y)/3, rect1.x + (rect2.x - rect1.x)/3)),
												(float)ComputeDis(disp,rect1.y + 2*(rect2.y - rect1.y)/3, rect1.x + 2*(rect2.x - rect1.x)/3)),
										max((float)ComputeDis(disp,rect1.y + (rect2.y - rect1.y)/3, rect1.x + (rect2.x - rect1.x)/3),
											(float)ComputeDis(disp,rect1.y + (rect2.y - rect1.y)/3, rect1.x + 2*(rect2.x - rect1.x)/3))) << " cm";
			
			string ShowDistText = DistText.str();
			cvPutText(img_detect, &ShowDistText[0], rect_dist, &font, cvScalar(255,255,0 ,  0));
			
			cvRectangle( img_detect, rect1, rect2, CV_RGB(0,255, 0),2, 8, 0 );
		}

		cvShowImage( "Binary Map",thres_img);	
		cvShowImage( "Detect Object",img_detect);
		
}


double StereoFunctions::ComputeDis(CvMat* Mat, int row, int col)
{
		double	focal = reprojectionVars[0];
		double	baseline = 6.2;//reprojectionVars[2];
		double	sensorSize = reprojectionVars[4];
		double 	k1  = 8e-7,
				k2  = -0.001,
				k3  = 0.4,
				k4  = 17.44;
		double 	depth = 0;
	//	if(cvGet2D(Mat,row,col).val[0]== 0) return 0;
		depth = (double)((baseline*focal)/((double)(cvGet2D(Mat,row,col).val[0]/16)));
		depth = (k1*depth*depth*depth + k2*depth*depth + k3*depth + k4);	
		return depth;
}

void StereoFunctions::stereoSavePointCloud()
{
		//0: fx(pixel), 1: fy(pixel), 2: base line (pixel), 3: f(mm), 4: sensor element size, 5: baseline (mm)
	double	focal = reprojectionVars[0];
	double	baseline = 6.2;//reprojectionVars[2];
	double	sensorSize = reprojectionVars[4];
	double 	k1  = 8e-7,
			k2  = -0.001,
			k3  = 0.4,
			k4  = 17.44;
		
	double 	depth = 0;
	real_disparity= cvCreateImage( imageSize, IPL_DEPTH_32F, 1 );
	cvConvertScale( disp, real_disparity, 1.0/16, 0 );
	
	//Measure distance from depth map
	for(int y=0;y<vdisp->rows;y++)
	{
		for(int x=0;x<vdisp->cols;x++)
		{
			if(cvGet2D(vdisp,y,x).val[0]>0){
			//  depth = atan(PI/2 - atan((cvGet2D(vdisp,y,x).val[0])/focal))*baseline;
				depth = (double)((baseline*focal)/((double)(cvGet2D(disp,y,x).val[0]/16)));
				depth = (k1*depth*depth*depth + k2*depth*depth + k3*depth + k4);
			//	depth = Interpol->result(depth);	
			}else
			depth=0;
		cvmSet(depthM,y,x,depth); 
		}
	}
if(SAVE_DISTANCE){
	FILE *distanceFile;
	stringstream strDistance;
	strDistance<<"Distance/Distance.dat";
	string Distance = strDistance.str();
	distanceFile = fopen(&Distance[0],"w");

	for (int y = 0; y < vdisp->rows; y++){
		for (int x = 0; x < vdisp->cols ; x++){

				fprintf(distanceFile, "%d %d %f %f\n",y,x,(float)cvmGet(depthM,y,x),(float)cvGet2D(real_disparity,y,x).val[0]);
			}
	}
fclose(distanceFile);
}

}

void StereoFunctions::stereoCalibration(StereoGrabber* grabb)
{

	int  nx=10, ny=7, frame = 0, n_boards =40, N;
	int count1 = 0,count2 = 0, result1=0, result2=0;	
    int  successes1 = 0,successes2 = 0 ;
   	const int maxScale = 1;
	const float squareSize = 0.023f;		//Set this to your actual square size
	CvSize imageSize = {0,0};
	CvSize board_sz = cvSize( nx,ny );
	
	int i, j, n = nx*ny, N1 = 0, N2 = 0;
    vector<CvPoint2D32f> points[2];
	vector<int> npoints;
	vector<CvPoint3D32f> objectPoints;
	vector<CvPoint2D32f> temp1(n); 
	vector<CvPoint2D32f> temp2(n);
    
    double M1[3][3], M2[3][3], D1[5], D2[5];
    double R[3][3], T[3], E[3][3], F[3][3];
	double Q[4][4];
	CvMat _Qcalib  = cvMat(4,4, CV_64F, Q);
    CvMat _M1calib = cvMat(3, 3, CV_64F, M1 );
    CvMat _M2calib = cvMat(3, 3, CV_64F, M2 );
    CvMat _D1 	   = cvMat(1, 5, CV_64F, D1 );
    CvMat _D2      = cvMat(1, 5, CV_64F, D2 );
    CvMat _R       = cvMat(3, 3, CV_64F, R );
    CvMat _Tcalib  = cvMat(3, 1, CV_64F, T );
    CvMat _E       = cvMat(3, 3, CV_64F, E );
    CvMat _F       = cvMat(3, 3, CV_64F, F );
	
	//Start webcam
		printf("\nWebcams are starting ...\n");
		grabb->stereoGrabberInitFrames();
		grabb->stereoGrabberGrabFrames();
		IplImage *frame1 = grabb->imageLeft;
		IplImage* gray_fr1 = cvCreateImage( cvGetSize(frame1), 8, 1 );
		IplImage *frame2 = grabb->imageRight;
		IplImage* gray_fr2 = cvCreateImage( cvGetSize(frame2), 8, 1 );
		imageSize = cvGetSize(frame1);
		
	//Thu anh chessboard ve cho viec calib camera	
		cvNamedWindow( "camera2", 1 );
		cvNamedWindow( "camera1", 1 );
		cvNamedWindow("corners camera1",1);
		cvNamedWindow("corners camera2",1);	
		printf("\nWant to capture %d chessboards for calibrate:", n_boards);	
		while((successes1<n_boards)||(successes2<n_boards))						
	{
	    	
	//--------Find and Draw chessboard--------------------------------------------------	
			
		if((frame++ % 20) == 0)
		{
			//----------------CAM1-------------------------------------------------------------------------------------------------------
			result1 = cvFindChessboardCorners( frame1, board_sz,&temp1[0], &count1,CV_CALIB_CB_ADAPTIVE_THRESH|CV_CALIB_CB_FILTER_QUADS);
			cvCvtColor( frame1, gray_fr1, CV_BGR2GRAY );
			//cvEqualizeHist(gray_fr1,gray_fr1);

			//----------------CAM2--------------------------------------------------------------------------------------------------------
			result2 = cvFindChessboardCorners( frame2, board_sz,&temp2[0], &count2,CV_CALIB_CB_ADAPTIVE_THRESH|CV_CALIB_CB_FILTER_QUADS);
			cvCvtColor( frame2, gray_fr2, CV_BGR2GRAY );
			//cvEqualizeHist(gray_fr2,gray_fr2);
			if(count1==n&&count2==n&&result1&&result2)
			{
					cvFindCornerSubPix( gray_fr1, &temp1[0], count1,cvSize(11, 11), cvSize(-1,-1),cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,30, 0.01) );
					cvDrawChessboardCorners( frame1, board_sz, &temp1[0], count1, result1 );
					cvShowImage( "corners camera1", frame1 );
					N1 = points[0].size();
					points[0].resize(N1 + n, cvPoint2D32f(0,0));
					copy( temp1.begin(), temp1.end(), points[0].begin() + N1 );
					++successes1;
					
					cvFindCornerSubPix( gray_fr2, &temp2[0], count2,cvSize(11, 11), cvSize(-1,-1),cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,30, 0.01) );
					cvDrawChessboardCorners( frame2, board_sz, &temp2[0], count2, result2 );
					cvShowImage( "corners camera2", frame2 );
					N2 = points[1].size();
					points[1].resize(N2 + n, cvPoint2D32f(0,0));
					copy( temp2.begin(), temp2.end(), points[1].begin() + N2 );
					++successes2;
					
					printf("\nNumber of couple Chessboards were found: %d", successes2);
			}
			
			else
			{		cvShowImage( "corners camera2", gray_fr2 );	
					cvShowImage( "corners camera1", gray_fr1 );	
			}
			

			grabb->stereoGrabberGrabFrames();
			frame1 = grabb->imageLeft;
			cvShowImage("camera1", frame1);
			frame2 = grabb->imageRight;
			cvShowImage("camera2", frame2);
			
			if(cvWaitKey(15)==27) break;
		}
	}
	
		grabb->stereoGrabberStopCam();
		cvDestroyWindow("camera1");
		cvDestroyWindow("camera2");
		cvDestroyWindow("corners camera1");
		cvDestroyWindow("corners camera2");	
		printf("\nDone Capture!");
		
		
		//--------------Compute for calibration-------------------
		N = n_boards*n;
		objectPoints.resize(N);
		for( i = 0; i < ny; i++ )
			for(j = 0; j < nx; j++ )   objectPoints[i*nx + j] = cvPoint3D32f(i*squareSize, j*squareSize, 0);
		for( i = 1; i < n_boards; i++ ) copy( objectPoints.begin(), objectPoints.begin() + n, objectPoints.begin() + i*n );
		npoints.resize(n_boards,n);
		
		CvMat _objectPoints = cvMat(1, N, CV_32FC3, &objectPoints[0] );
		CvMat _imagePoints1 = cvMat(1, N, CV_32FC2, &points[0][0] );
		CvMat _imagePoints2 = cvMat(1, N, CV_32FC2, &points[1][0] );
		CvMat _npoints = cvMat(1, npoints.size(), CV_32S, &npoints[0] );
		cvSetIdentity(&_M1calib);
		cvSetIdentity(&_M2calib);
		cvZero(&_D1);
		cvZero(&_D2);
		
		printf("\nRunning stereo calibration ...");
		fflush(stdout);
		cvStereoCalibrate( &_objectPoints, &_imagePoints1, &_imagePoints2, &_npoints,&_M1calib, &_D1, &_M2calib, &_D2,imageSize, &_R, &_Tcalib, &_E, &_F,
		cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-5),
        CV_CALIB_FIX_ASPECT_RATIO+CV_CALIB_ZERO_TANGENT_DIST + CV_CALIB_SAME_FOCAL_LENGTH );


		printf("\nDone Calibration");
		//-------------Chinh meo cho anh------------------------------------------
		cvUndistortPoints( &_imagePoints1, &_imagePoints1,&_M1calib, &_D1, 0, &_M1calib );
		cvUndistortPoints( &_imagePoints2, &_imagePoints2,&_M2calib, &_D2, 0, &_M2calib );
		
		//--------Tinh toan va canh anh, dung giai thuat bouguet-------------------
		CvMat* mx1calib = cvCreateMat( imageSize.height,imageSize.width, CV_32F );
        CvMat* my1calib = cvCreateMat( imageSize.height,imageSize.width, CV_32F );
        CvMat* mx2calib = cvCreateMat( imageSize.height,imageSize.width, CV_32F );
        CvMat* my2calib = cvCreateMat( imageSize.height,imageSize.width, CV_32F );

        double R1[3][3], R2[3][3], P1[3][4], P2[3][4];
        CvMat _R1 = cvMat(3, 3, CV_64F, R1);
        CvMat _R2 = cvMat(3, 3, CV_64F, R2);
		//Calib dung Bouguet
            CvMat _P1 = cvMat(3, 4, CV_64F, P1);
            CvMat _P2 = cvMat(3, 4, CV_64F, P2);
            cvStereoRectify( &_M1calib, &_M2calib, &_D1, &_D2, imageSize,&_R, &_Tcalib,&_R1, &_R2, &_P1, &_P2, &_Qcalib,0/*CV_CALIB_ZERO_DISPARITY*/ );
        //Tim tham so cho cvRemap()
            cvInitUndistortRectifyMap(&_M1calib,&_D1,&_R1,&_P1,mx1calib,my1calib);
            cvInitUndistortRectifyMap(&_M2calib,&_D2,&_R2,&_P2,mx2calib,my2calib);

			printf("\nSaving matries for later use ...\n");
			cvSave("CalibFile//M1.yml",&_M1calib);
		//	cvSave("CalibFile//D1.yml",&_D1);
		//	cvSave("CalibFile//R1.yml",&_R1);
		//	cvSave("CalibFile//P1.yml",&_P1);
			cvSave("CalibFile//M2.yml",&_M2calib);
		//	cvSave("CalibFile//D2.yml",&_D2);
		//	cvSave("CalibFile//R2.yml",&_R2);
		//	cvSave("CalibFile//P2.yml",&_P2);
			cvSave("CalibFile//Q.yml",&_Qcalib);
			cvSave("CalibFile//T.yml",&_Tcalib);
			cvSave("CalibFile//mx1.yml",mx1calib);
			cvSave("CalibFile//my1.yml",my1calib);
			cvSave("CalibFile//mx2.yml",mx2calib);
			cvSave("CalibFile//my2.yml",my2calib);

}



	
