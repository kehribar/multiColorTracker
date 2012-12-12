/*
* ----------------------------------------------------------------------------
* “THE COFFEEWARE LICENSE” (Revision 1):
* <ihsan@kehribar.me> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a coffee in return.
* -----------------------------------------------------------------------------
*/
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

using namespace cv;
using namespace std;

/******************************************************************************
/ Macro definitions
/*****************************************************************************/
#define COLOR_RESOLUTION 8 /* 8 bits -> 256 colors ... */
#define setBit(register,bit) (register |= (1<<bit))
#define clearBit(register,bit) (register &= ~(1<<bit))
#define checkBit(register,bit) ((register&(1<<bit))>>bit)
/*****************************************************************************/

/******************************************************************************
/ Target struct definition
/*****************************************************************************/
struct target {
	int center_x; /* user read only */
	int center_y; /* user read only */
	int totalArea; /* user read only */
	int	target_id; /* user writable */
	int minimumArea; /* user writable */
	Mat* binary_image; /* user writable */
	float colorTolerance[3]; /* user writable */
	unsigned char centerColor[3]; /* user writable */
};
/*****************************************************************************/

/******************************************************************************
/ Update classification
/	i: row_id
/	j: col_id
/*****************************************************************************/
void update_classification(Mat* inputImage, int i,int j);
/*****************************************************************************/

/******************************************************************************
/ Updates binary mask for a spesific target ID
/	i: row_id
/	j: col_id
/*****************************************************************************/
void update_binaryImage(target* targetObject, int i, int j);
/*****************************************************************************/

/******************************************************************************
/ Creates a binary mask for speed color based classification
/*****************************************************************************/
void update_classMask(target* targetObject);
/*****************************************************************************/

/*****************************************************************************
/ Compute center of mass
/****************************************************************************/
void compute_centerPoint(target* targetObject);
/****************************************************************************/

/*****************************************************************************
/ Bitwise test functions 
/ 	-- unneccessary after the initial test
/	-- used for mental check
/****************************************************************************/
void bitwise_test();
/*****************************************************************************/
