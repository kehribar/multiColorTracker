/******************************************************************************
/ OpenCV trial - <ihsan@kehribar.me> - July 2012
/ 	Initial testing platform for embedded vision platform.	
/*****************************************************************************/
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "multiColorTracker.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

using namespace cv;
using namespace std;

#define AVERAGE 2
#define DOWNSAMPLE 8
#define AREA_TRESHOLD 1
#define GAUSSIAN_KERNEL 5

int state = 0;
int tolerance_1 = 40;
int tolerance_2 = 15;
int tolerance_3 = 15;
unsigned char pickedColor[3];
Mat globalFrame = Mat::ones(480/DOWNSAMPLE,640/DOWNSAMPLE,CV_8U)*128;

/* Tracking object definitons */
target qTarget;
target xTarget;

/******************************************************************************
/ Picks a color from an input Mat(rix) data type. 'x' and 'y' are the desired
/ coordinates. 'output' is the arbitrary length array where the color info.
/ will be stored. average is the number of neighbour pixels we should take
/ into consideration.
/*****************************************************************************/
void pickColor(Mat input, int x, int y, int average, unsigned char* output)
{
	int num = input.channels();
	int temp[num];
	
	for(int i=0;i<num;i++)
	{
		temp[i] = 0x00;
		for(int q=y-average;q<y+average+1;q++)
		{
			for(int qq=x-average;qq<x+average+1;qq++)
			{
				temp[i] += globalFrame.at<Vec3b>(q,qq)[i];
			}
		}
		output[i] = temp[i] / (((2*average)+1)*((2*average)+1));
	}
}
/*****************************************************************************/

/*****************************************************************************
/ On mouse event.
/****************************************************************************/
static void onMouse( int event, int x, int y, int, void* )
{
	switch( event )
	{
		case EVENT_LBUTTONDOWN:
			pickColor(globalFrame,x,y,AVERAGE,pickedColor);

			xTarget.centerColor[0] = pickedColor[0];
			xTarget.centerColor[1] = pickedColor[1];
			xTarget.centerColor[2] = pickedColor[2];

			update_classMask(&xTarget);
			break;
		case EVENT_RBUTTONDOWN:
			pickColor(globalFrame,x,y,AVERAGE,pickedColor);

			qTarget.centerColor[0] = pickedColor[0];
			qTarget.centerColor[1] = pickedColor[1];
			qTarget.centerColor[2] = pickedColor[2];

			update_classMask(&qTarget);
			break;
		case EVENT_LBUTTONUP:
			break;
		case EVENT_RBUTTONUP:
			break;			
	}

}
/*****************************************************************************/

/*****************************************************************************
/ Trackbar event function
/****************************************************************************/
void on_trackbar( int, void* )
{
	xTarget.colorTolerance[0] = tolerance_1 / 100.0;
	xTarget.colorTolerance[1] = tolerance_2 / 200.0;
	xTarget.colorTolerance[2] = tolerance_3 / 200.0;

	qTarget.colorTolerance[0] = tolerance_1 / 100.0;
	qTarget.colorTolerance[1] = tolerance_2 / 200.0;
	qTarget.colorTolerance[2] = tolerance_3 / 200.0;

	update_classMask(&xTarget);
	update_classMask(&qTarget);
}
/*****************************************************************************/

/*****************************************************************************
/ Main function
/****************************************************************************/
int main( int argc, const char** argv )
{

	if ( argc != 2)
	{
		printf(" USAGE: to use first connected camera to your system give 0 as a parameter \n or you can give direct ip adress of video stream \n <rtsp://140.114.188.219/axis-media/media.amp>\n");
		return -1;

	}
	/* Various frame definitions "http://192.168.1.100:8081/?action=stream" */
	VideoCapture cap(argv[1],0);
	Mat frame = Mat::ones(511,511,CV_8UC3);
	Mat myColor = Mat::ones(32,32,CV_8UC3);
	Mat frame_hsv = Mat::ones(480,640,CV_8UC3);
	Mat	myOutput = Mat::ones(480/DOWNSAMPLE,640/DOWNSAMPLE,CV_8UC3);
	Mat raw_input = Mat::ones(480/DOWNSAMPLE,640/DOWNSAMPLE,CV_8UC3);
	Mat binary_image = Mat::zeros(480/DOWNSAMPLE,640/DOWNSAMPLE,CV_8U);
	Mat binary_image2 = Mat::zeros(480/DOWNSAMPLE,640/DOWNSAMPLE,CV_8U);
	Mat	myOutput_rgb = Mat::ones(480/DOWNSAMPLE,640/DOWNSAMPLE,CV_8UC3);

	/* initialize objects */
	qTarget.target_id = 0x00;
	qTarget.binary_image = &binary_image;
	qTarget.colorTolerance[0] = tolerance_1 / 100.0;
	qTarget.colorTolerance[1] = tolerance_2 / 200.0;
	qTarget.colorTolerance[2] = tolerance_3 / 200.0;

	/* initialize objects */
	xTarget.target_id = 0x01;
	xTarget.binary_image = &binary_image2;	
	xTarget.colorTolerance[0] = tolerance_1 / 100.0;
	xTarget.colorTolerance[1] = tolerance_2 / 200.0;
	xTarget.colorTolerance[2] = tolerance_3 / 200.0;

	/* Open the first available camera */
	//cap.open(0);

	if(!cap.isOpened())	
		cout << "Camera init. problem!*\n";

	/* Create windows */
	namedWindow("directCamera", 0);
	namedWindow("selected_color",0);

	/* Create GUI elements */
	setMouseCallback("directCamera", onMouse, 0);
	createTrackbar( "#1", "directCamera", &tolerance_1, 50, on_trackbar);
	createTrackbar( "#2", "directCamera", &tolerance_2, 50, on_trackbar);
	createTrackbar( "#3", "directCamera", &tolerance_3, 50, on_trackbar);

	for(;;)
	{
		/* Grab the frame */
		cap >> frame;
		
		if(frame.empty())	
			break;

		/* Convert the camera frame to YUV color space */
		cvtColor(frame,frame_hsv,COLOR_RGB2YCrCb,3);

		/* Downsize the image according to the DOWNSAMPLE definition */
 		resize(frame_hsv,raw_input,raw_input.size(),0,0,INTER_AREA);

 		/* Apply a gaussian blur to the image */
 		GaussianBlur(raw_input,globalFrame,Size(GAUSSIAN_KERNEL,GAUSSIAN_KERNEL),0,0,BORDER_DEFAULT);

 		/* Copy the original image to a seperate buffer for color picking */
		globalFrame.copyTo(myOutput);

		/* Pointwise processing & classification */	
		for(int i=0; i<myOutput.rows; i++)
		{
			for(int j=0; j<myOutput.cols; j++)
			{
				update_classification(&myOutput,i,j);
				update_binaryImage(&xTarget,i,j);
				update_binaryImage(&qTarget,i,j);
			}
		}
		
		/* Compute the center points */
		compute_centerPoint(&xTarget);
		compute_centerPoint(&qTarget);

		circle( /* Overlay a circle image at the center of xTarget */
			myOutput,
			Point(xTarget.center_x,xTarget.center_y),
			3,
			Scalar(0,0,0),
			1, /* thickness */
			8 /* line type */
		);

		circle( /* Overlay a circle image at the center of qTarget */
			myOutput,
			Point(qTarget.center_x,qTarget.center_y),
			3,
			Scalar(255,255,0),
			1, /* thickness */
			8 /* line type */
		);

		/* Update selected color window */
		for(int i=0; i<myColor.rows; i++)
		{
			for(int j=0; j<myColor.cols; j++)
			{
				myColor.at<Vec3b>(i,j)[0] = (unsigned char) pickedColor[0];
				myColor.at<Vec3b>(i,j)[1] = (unsigned char) pickedColor[1];
				myColor.at<Vec3b>(i,j)[2] = (unsigned char) pickedColor[2];
			}
		}

		/* Convert selected YUV color back to RGB */
		cvtColor(myColor,myColor,COLOR_YCrCb2RGB,3);
		imshow("selected_color",myColor);

		/* Convert image back to RGB */
		cvtColor(myOutput,myOutput_rgb,COLOR_YCrCb2RGB,3);

		/* Toggle between overlayered RGB image and binary images */
		if(state == 0)
			imshow("directCamera", myOutput_rgb);
		else if(state == 1)
			imshow("directCamera", binary_image);
		else if(state == 2)
			imshow("directCamera", binary_image2);

		/* Keyboard input routine */
		switch(waitKey(1))
		{
			case 'q':
				return 0;
				break;
			case 'x':
				state += 1;
				if(state==3) 
					state = 0;
				break;
			default:
				break;
		}
	}

	return 0;
}
/*****************************************************************************/