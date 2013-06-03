#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

Mat sourceImage;
unsigned char thres;
unsigned theoreticalLedSize;
Mat channel[3];
Point corners[4];
vector<Vec3f> circles;

void preprocessImage()
{
	split(sourceImage, channel);
	MatIterator_<uchar> it, end;
	GaussianBlur( channel[2], channel[2], Size(9, 9), 2, 2 );

	for( it = channel[2].begin<uchar>(), end = channel[2].end<uchar>(); it != end; ++it)
	{
		(*it)=((*it)>thres)?255:0;
	}
	channel[0] = Scalar(0);
	channel[1] = Scalar(0);

}

inline unsigned L1Distance(Point a, Point b)
{
	return abs(a.x - b.x) + abs(a.y - b.y);
}

inline double L2Distance(Point a, Point b)
{
	return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

inline unsigned satSub(int a, int b)
{
	return (a<b)?0:a-b;
}

void findCorners()
{
	Point imageCorners[4];
	imageCorners[0] = Point(0,0);
	imageCorners[1] = Point(sourceImage.cols,0);
	imageCorners[2] = Point(0, sourceImage.rows);
	imageCorners[3] = Point(sourceImage.cols, sourceImage.rows);

	corners[0] = corners[1] = corners[2] = corners[3] = Point(circles[0][0], circles[0][1]);
	for (size_t i = 1; i < circles.size(); ++i) {
		Point currentPoint(circles[i][0], circles[i][1]);
		for (size_t j = 0; j < 4; ++j) {
			if (L1Distance(currentPoint, imageCorners[j]) < L1Distance(corners[j], imageCorners[j])) {
				corners[j] = currentPoint;
			}
		}
	}
}


Rect RectangleFromCorners(Point corners[4])
{
	int x1,y1,x2,y2;
	x1=x2=corners[0].x;
	y1=y2=corners[0].y;

	for (size_t i = 1; i<4; ++i) {
		int x=corners[i].x;
		int y=corners[i].y;

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

int main( int argc, char** argv )
{
	if( argc != 2)
	{
		cout <<" Usage: " << argv[0] << " display_image ImageToLoadAndDisplay" << endl;
		return -1;
	}

	sourceImage = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

	if(! sourceImage.data )                              // Check for invalid input
	{
		cout <<  "Could not open or find the image" << std::endl ;
		return -1;
	}
	theoreticalLedSize = sourceImage.cols/64;
	thres = 150;
	namedWindow( "Display window", CV_WINDOW_AUTOSIZE );// Create a window for display.

	char key=0;
	do {
		switch (key) {
		case 'I':
			thres += 10;
			break;
		case 'D':
			thres -= 10;
			break;
		case 'i':
			thres += 1;
			break;
		case 'd':
			thres -= 1;
			break;
		}
		cout << "thres = " << (unsigned)thres << endl;

		preprocessImage();

		HoughCircles( channel[2], circles, CV_HOUGH_GRADIENT, 1, 20, 30, 10, 0, theoreticalLedSize );

		findCorners();
		for (size_t i = 0; i < 4; ++i) {
			circle(channel[0], corners[i], 10, Scalar(255), -1, 8, 0);
		}

		for( size_t i = 0; i < circles.size(); i++ )
		{
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
			// circle center
			circle(  channel[1], center, 3, Scalar(255), -1, 8, 0 );
			// circle outline
			circle(  channel[1], center, radius, Scalar(255), 3, 8, 0 );
		}

		cout << "Found " << circles.size() << " circles." << endl;

		//calculate bounding rect
		Rect myROI = RectangleFromCorners(corners);
		myROI.x = satSub(myROI.x, 20);
		myROI.y = satSub(myROI.y, 20);
		myROI.width += 40;
		myROI.height += 40;
		cout << "Bounding rect " << myROI.x << " " << myROI.y << " " <<
				myROI.width << " " << myROI.height << endl;
		Mat result;
		merge(channel, 3, result);
		imshow( "Display window", result(myROI) );                   // Show our image inside it.
	} while ((key=waitKey(0)) != 'q');                                          // Wait for a keystroke in the window

	return 0;
}
