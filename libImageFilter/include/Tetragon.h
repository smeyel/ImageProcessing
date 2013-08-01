#ifndef __TETRAGON_H
#define __TETRAGON_H

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)

using namespace cv;

namespace smeyel
{
	/** Represents a tetragon, an area with 4 corners, but not necessarily a rectangle. */
	class Tetragon
	{
	public:
		Point2f corners[4];	// Upper left, upper right, lower left, lower right

		void draw(Mat &img, Scalar color);

		void getPolyline(Point *targetOf4Points);

		// Modifies corners!
		void compensatePerspectiveTransform(Mat &src, Mat &dst);

		void addCodedMask(Mat &dst, unsigned char colorCode);
	};
}

#endif