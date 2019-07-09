# Stereo vision experiment on cheap cameras
This project was implemented on VS2008 with OpenCV2.1.

How to run this?

+ It needs cvBlobsLib. For setup info, get here: http://dsynflo.blogspot.com/2010/02/cvblobskib-with-opencv-installation.html
  Here's code for my test using cvblobs: http://dl.dropbox.com/u/110310945/Blobs%20test.rar

+ Distance between two cameras in my project is 6 cm, you can chose father distance for the best result with your camera's type

+ Before running "Detect objects & compute distance to object" module, you must calibrate your cameras. How?
  Change "#define CALIBRATION 0" to "#define CALIBRATION 1" in stdafx.h file.
  After calibration, you will get new matrices in "CalibFile" folder (*.yml), if the result is good enough, change back "#define CALIBRATION 0"
  Put this line into file stdafx.h: "#define ANALYSIS_MODE 1" as well.

+ I used chessboard 10x7 & 40 frames for calibration module, more details in "StereoFunctions.cpp" file. You can change these numbers suitable with your type of chessboard.

+ For problems with loading cameras, you should modify in "StereoGrabber.cpp". Put "index" appropriate with your device in cvCaptureFromCAM(index) function.

+ About computing distance: it interpolates the relationship between depth-value and real-distance to third degree polynomial. So i used excel file "interpolation" for interpolation to find k1 to k4, you should find your own value of these parameters.

+ For the best result, you should adjust parameters in Stereo Controls window.

+ This package includes matlab files in "Distance" folder, feel free to edit it.

+ For the basic theory, read this paper (also include his full code): http://scholar.lib.vt.edu/theses/available/etd-12232009-222118/unrestricted/Short_NJ_T_2009.pdf
