/*
* ----------------------------------------------------------------------------
* “THE COFFEEWARE LICENSE” (Revision 1):
* <ihsan@kehribar.me> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a coffee in return.
* -----------------------------------------------------------------------------
*/

/******************************************************************************
/ include ...
/*****************************************************************************/
#include "multiColorTracker.h"
/*****************************************************************************/

/******************************************************************************
/ Data definitions
/*****************************************************************************/
static unsigned int yMask[2<<COLOR_RESOLUTION];
static unsigned int uMask[2<<COLOR_RESOLUTION];
static unsigned int vMask[2<<COLOR_RESOLUTION];
static unsigned int classificationResult=0x00;
/*****************************************************************************/

/******************************************************************************
/ Update classification
/	i: row_id
/	j: col_id
/*****************************************************************************/
void update_classification(Mat* inputImage, int i,int j)
{
	classificationResult = yMask[inputImage->at<Vec3b>(i,j)[0]] & uMask[inputImage->at<Vec3b>(i,j)[1]] & vMask[inputImage->at<Vec3b>(i,j)[2]];
}
/*****************************************************************************/

/******************************************************************************
/ Updates binary mask for a spesific target ID
/	i: row_id
/	j: col_id
/*****************************************************************************/
void update_binaryImage(target* targetObject, int i, int j)
{
	if(checkBit(classificationResult,targetObject->target_id))
	{
		(targetObject->binary_image)->at<uchar>(i,j) = 0xFF;					
	}
	else
	{
		(targetObject->binary_image)->at<uchar>(i,j) = 0x00;
	}
}
/*****************************************************************************/

/******************************************************************************
/ Creates a binary mask for speed color based classification
/*****************************************************************************/
void update_classMask(target* targetObject)
{
	unsigned int margin;
	int min_1,min_2,min_3;
	int max_1,max_2,max_3;

	/* Calculate max and min values */
	margin = ((targetObject->colorTolerance[0])*(2<<COLOR_RESOLUTION));
	max_1=targetObject->centerColor[0]+margin;
	min_1=targetObject->centerColor[0]-margin;
	if(max_1>(2<<COLOR_RESOLUTION))
		max_1=2<<COLOR_RESOLUTION;
	if(min_1<0)
		min_1=0;

	/* Calculate max and min values */
	margin = ((targetObject->colorTolerance[1])*(2<<COLOR_RESOLUTION));
	max_2=targetObject->centerColor[1]+margin;
	min_2=targetObject->centerColor[1]-margin;
	if(max_2>(2<<COLOR_RESOLUTION))
		max_2=2<<COLOR_RESOLUTION;
	if(min_2<0)
		min_2=0;

	/* Calculate max and min values */
	margin = ((targetObject->colorTolerance[2])*(2<<COLOR_RESOLUTION));
	max_3=targetObject->centerColor[2]+margin;
	min_3=targetObject->centerColor[2]-margin;
	if(max_3>(2<<COLOR_RESOLUTION))
		max_3=2<<COLOR_RESOLUTION;
	if(min_3<0)
		min_3=0;				

	printf("\r\n");
	printf("-- updated color margin values: \r\n");
	printf("#id:    %3d\r\n",targetObject->target_id);
	printf("#max_1: %3d\t#min_1: %3d\n",max_1,min_1);
	printf("#max_2: %3d\t#min_2: %3d\n",max_2,min_2);
	printf("#max_3: %3d\t#min_3: %3d\n",max_3,min_3);

	/* Update the color classification mask */
	for(int x=0;x<(2<<COLOR_RESOLUTION);x++)
	{
		if((x>min_1)&&(x<max_1))
			setBit(yMask[x],targetObject->target_id);
		else
			clearBit(yMask[x],targetObject->target_id);

		if((x>min_2)&&(x<max_2))
			setBit(uMask[x],targetObject->target_id);
		else
			clearBit(uMask[x],targetObject->target_id);

		if((x>min_3)&&(x<max_3))
			setBit(vMask[x],targetObject->target_id);
		else
			clearBit(vMask[x],targetObject->target_id);
	}

}
/*****************************************************************************/

/*****************************************************************************
/ Compute center of mass
/****************************************************************************/
void compute_centerPoint(target* targetObject)
{
	int im_row = (targetObject->binary_image)->rows;
	int im_col = (targetObject->binary_image)->cols;
	int totalX = 0x00;
	int totalY = 0x00;
	int totalArea = 0x00;

	/* firstly, compute the area */
	for(int i=0;i<im_row;i++)
	{
		for(int q=0;q<im_col;q++)
		{
			if((targetObject->binary_image)->at<uchar>(i,q)!=0x00)
			{
				(targetObject->binary_image)->at<uchar>(i,q) = 0x01;
				totalArea = totalArea + 1;
			}
		}
	}

	/* compute the center points */
	for(int i=0;i<im_row;i++)
	{
		for(int q=0;q<im_col;q++)
		{
			totalX += q * (targetObject->binary_image)->at<uchar>(i,q);
			totalY += i * (targetObject->binary_image)->at<uchar>(i,q);
		}
	}	

	/* restore image back */
	for(int i=0;i<im_row;i++)
	{
		for(int q=0;q<im_col;q++)
		{
			(targetObject->binary_image)->at<uchar>(i,q) = (targetObject->binary_image)->at<uchar>(i,q) * 0xFF;
		}
	}	

	if(totalArea!=0x00)
	{
		targetObject->center_x = totalX / totalArea;	
		targetObject->center_y = totalY / totalArea;
	}
	else
	{
		targetObject->center_x = 0x00;	
		targetObject->center_y = 0x00;
	}

}
/****************************************************************************/

/*****************************************************************************
/ Bitwise test functions 
/ 	-- unneccessary after the initial test
/	-- used for mental check
/****************************************************************************/
void bitwise_test()
{
	unsigned int testValue = 0x00;
	unsigned int errorCount = 0x00;

	for(int i=0;i<32;i++)
	{
		printf("------------------------\r\n");
		printf("-------- #%2d -----------\r\n",i);
		setBit(testValue,i);
		printf("Start val:\t%8X\r\n",testValue);
		if(checkBit(testValue,i)==0x00)
		{
			printf("Error at set bit\r\n");
			errorCount++;
		}
		clearBit(testValue,i);
		printf("End val:\t%8X\r\n",testValue);
		if(checkBit(testValue,i)!=0x00)
		{
			printf("Error at clear bit\r\n");
			errorCount++;
		}
	}
	printf("------------------------\r\n");
	printf("Test ends with %d error(s)\r\n",errorCount);

}
/*****************************************************************************/