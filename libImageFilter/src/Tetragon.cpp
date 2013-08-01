#include <opencv2\imgproc\imgproc.hpp>
#include "Tetragon.h"
using namespace smeyel;

void Tetragon::draw(Mat &img, Scalar color)
{
	line(img, corners[0], corners[1], color,3);
	line(img, corners[1], corners[3], color,3);
	line(img, corners[3], corners[2], color,3);
	line(img, corners[2], corners[0], color,3);
}

void Tetragon::getPolyline(Point *targetOf4Points)
{
	int idxReordering[] = { 0, 1, 3, 2 };	// Need circular order!
	for(int i=0; i<4; i++)
	{
		targetOf4Points[i].x = cvRound(this->corners[idxReordering[i]].x);
		targetOf4Points[i].y = cvRound(this->corners[idxReordering[i]].y);
	}
}

// Modifies corners!
void Tetragon::compensatePerspectiveTransform(Mat &src, Mat &dst)
{
	Point2f newCorners[4];
	newCorners[0] = Point2f(50,50);
	newCorners[1] = Point2f(640-50,50);
	newCorners[2] = Point2f(50,480-50);
	newCorners[3] = Point2f(640-50,480-50);

	Mat transform = getPerspectiveTransform(this->corners, newCorners);

	warpPerspective(src, dst, transform, Size(640,480));

	for(int i=0; i<4; i++)
	{
		this->corners[i] = newCorners[i];
	}
}

void Tetragon::addCodedMask(Mat &dst, unsigned char colorCode)
{
	OPENCV_ASSERT(dst.type() == CV_8UC1,"createMask", "Destination image is not CV_8UC1...");

	Point points[4];
	this->getPolyline(points);

	int n = 4;
	const Point *p = points;
	const Point **pp = &p;
		
	fillPoly(dst,pp,&n, 1, Scalar(colorCode));
}
