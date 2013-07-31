/*
 * TimeSyncBeaconReader.cpp
 *
 *  Created on: 2013.06.08.
 *      Author: norbi
 */

#include "TimeSyncBeaconReader.h"
#include "opencv2/imgproc/imgproc.hpp"
#ifdef DEBUG
#include <iostream>
#endif

#ifdef _WIN32
#define INFINITY	std::numeric_limits<double>::infinity()
#endif

using namespace cv;

TimeSyncBeaconReader::TimeSyncBeaconReader()
{
	thres = 150;
	LEDcountRow = 16;
}

unsigned char TimeSyncBeaconReader::getTreshold()
{
	return this->thres;
}

void TimeSyncBeaconReader::setThreshold(unsigned char thres)
{
	this->thres=thres;
}

unsigned TimeSyncBeaconReader::satSub(int a, int b)
{
	return (a<b)?0:a-b;
}


void TimeSyncBeaconReader::preprocessImage()
{
	split(sourceImage, channel);
	//GaussianBlur( channel[2], channel[2], Size(9, 9), 2, 2 );

	MatIterator_<uchar> it, end;
	for( it = channel[2].begin<uchar>(), end = channel[2].end<uchar>(); it != end; ++it)
	{
		(*it)=((*it)>thres)?255:0;
	}
	channel[0] = Scalar(0);
	channel[1] = Scalar(0);

}

void TimeSyncBeaconReader::findCorners()
{
	const SmartVector imageCorners[4] = { 	Point(0,0),
											Point(sourceImage.cols,0),
											Point(0, sourceImage.rows),
											Point(sourceImage.cols, sourceImage.rows)
	};

	// TODO: when circle[0] does not exist, it causes assertion fail...
	if (circles.size()==0)
	{
		corners[0] = imageCorners[0];
		corners[1] = imageCorners[1];
		corners[2] = imageCorners[2];
		corners[3] = imageCorners[3];
		return;
	}
	corners[0] = corners[1] = corners[2] = corners[3] = circles[0].getCenterPoint();
	for (size_t i = 1; i < circles.size(); ++i) {
		SmartVector currentPoint = circles[i].getCenterPoint();
		for (size_t j = 0; j < 4; ++j) {
			if (currentPoint.distance(imageCorners[j],1) < corners[j].distance(imageCorners[j],1)) {
				corners[j] = currentPoint;
			}
		}
	}
}


Rect TimeSyncBeaconReader::RectangleFromCorners(const SmartVector corners[4])
{
	int x1,y1,x2,y2;
	x1=x2=cvRound(corners[0].getX());
	y1=y2=cvRound(corners[0].getY());

	for (size_t i = 1; i<4; ++i) {
		int x=cvRound(corners[i].getX());
		int y=cvRound(corners[i].getY());

		if (x < x1) x1 = x;
		else if (x > x2) x2 = x;

		if (y < y1) y1 = y;
		else if (y > y2) y2 = y;
	}

	Rect retval;
	retval.x = x1;
	retval.y = y1;
	retval.width = satSub(x2,x1);
	retval.height = satSub(y2,y1);
	return retval;
}

void TimeSyncBeaconReader::GeneratePoints(const SmartVector &a, const SmartVector &b)
{
	for (int j = 0; j < LEDcountRow; ++j) {
		SmartVector newPoint;
		newPoint = a + (b-a)*((double)j/(LEDcountRow-1));
		LEDcenterPoints.push_back(newPoint.toPoint());
	}
}

void TimeSyncBeaconReader::GenerateLEDcenterPoints()
{
	SmartVector a, b, c, d;
	a=corners[0];
	b=corners[1];
	c=corners[2];
	d=corners[3];
	GeneratePoints(a,b);
	GeneratePoints(a+(c-a)*((double)3/31), b+(d-b)*((double)3/31));
	GeneratePoints(a+(c-a)*((double)28/31), b+(d-b)*((double)28/31));
	GeneratePoints(c,d);
}

void TimeSyncBeaconReader::CreateDavidArray()
{
	//Find the biggest circle
	float maxRadius = 0;
	for (size_t cp = 0; cp < circles.size(); ++cp)
		if (circles[cp].getR() > maxRadius) maxRadius = circles[cp].getR();

#ifdef _WIN32
	memset(davidArray, 0, sizeof(davidArray));
#else
	bzero(davidArray, sizeof(davidArray));
#endif
	for (size_t cp = 0; cp < circles.size(); ++cp) { //cp stands for circle center point index
		double min = INFINITY;
		size_t mini = (size_t)-1;
		for (size_t lp = 0; lp < LEDcenterPoints.size(); ++lp) { //lp stands for LED center point index
			double distance = circles[cp].getCenterPoint().distance(LEDcenterPoints[lp]);
			if (min > distance) {
				min = distance;
				mini=lp;
			}
		}
		if (mini != (size_t)-1) {
			davidArray[63-mini] = (unsigned int)((circles[cp].getR()/maxRadius)*255);
		}
	}
}

void TimeSyncBeaconReader::ProcessImage(const cv::Mat &srcImg)
{
	sourceImage = srcImg;
	circles.clear();
	LEDcenterPoints.clear();
	theoreticalLedSize = sourceImage.cols/32;

	preprocessImage();
	vector<Vec3f> tmpCircles;
	HoughCircles( channel[2], tmpCircles, CV_HOUGH_GRADIENT, 1, 20, 30, 7, 0, theoreticalLedSize );
#ifdef DEBUG
	std::cout << "DEBUG Found " << tmpCircles.size() << " circles." << std::endl;
#endif
	for (size_t i = 0; i < tmpCircles.size(); ++i) {
		circles.push_back(tmpCircles[i]);
	}
	findCorners();

	GenerateLEDcenterPoints();
	CreateDavidArray();
}


cv::Rect TimeSyncBeaconReader::getBoundingRect()
{
	Rect myROI;
	myROI = RectangleFromCorners(corners);
	myROI.x = satSub(myROI.x, 20);
	myROI.y = satSub(myROI.y, 20);
	myROI.width += 40;
	myROI.height += 40;
	return myROI;
}

void TimeSyncBeaconReader::GenerateImage()
{
	for (size_t i = 0; i < 4; ++i) {
		circle(channel[0], corners[i].toPoint(), 10, Scalar(255), -1, 8, 0);
	}

	for( size_t i = 0; i < circles.size(); i++ )
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		// circle center
		circle(  channel[1], center, 3, Scalar(255), -1, 8, 0 );
		// circle outline
		circle(  channel[1], center, radius, Scalar(255), 3, 8, 0 );
		circle(  channel[0], center, radius, Scalar(0), 3, 8, 0 );
		circle(  channel[2], center, radius, Scalar(0), 3, 8, 0 );
	}
	channel[2] = Scalar(0);
	for (size_t i = 0; i < LEDcenterPoints.size(); ++i) {
		circle(channel[2], LEDcenterPoints.at(i).toPoint(), 3, Scalar(255), -1, 8, 0);
	}

}

