#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

Mat sourceImage;

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

	namedWindow( "Display window", CV_WINDOW_AUTOSIZE );// Create a window for display.

	char key=0;
	unsigned char threshold=150;
	do {
		switch (key) {
		case 'I':
			threshold += 10;
			break;
		case 'D':
			threshold -= 10;
			break;
		case 'i':
			threshold += 1;
			break;
		case 'd':
			threshold -= 1;
			break;
		}
		cout << "Threshold = " << (unsigned)threshold << endl;

		unsigned theoreticalLedSize = sourceImage.cols/64;
		Mat channel[3];
		split(sourceImage, channel);
		MatIterator_<uchar> it, end;
		GaussianBlur( channel[2], channel[2], Size(9, 9), 2, 2 );

		for( it = channel[2].begin<uchar>(), end = channel[2].end<uchar>(); it != end; ++it)
		{
			(*it)=((*it)>threshold)?255:0;
		}
		channel[0] = Scalar(0);
		channel[1] = Scalar(1);

		vector<Vec3f> circles;
		HoughCircles( channel[2], circles, CV_HOUGH_GRADIENT, 1, 20, 30, 10, 0, theoreticalLedSize );
		Point leftmost(channel[2].cols,0), rightmost(0,0),
				topmost(0,channel[2].rows), bottommost(0,0);
		for (size_t i = 0; i < circles.size(); ++i) {
			if (cvRound(circles[i][0]) < leftmost.x)
				leftmost = Point(circles[i][0], circles[i][1]);
			if (cvRound(circles[i][0]) > rightmost.x)
				rightmost = Point(circles[i][0], circles[i][1]);
			if (cvRound(circles[i][1]) < topmost.y)
				topmost = Point(circles[i][0], circles[i][1]);
			if (cvRound(circles[i][1]) > bottommost.y)
				bottommost = Point(circles[i][0], circles[i][1]);
		}
		circle(channel[0], leftmost, 10, Scalar(255), -1, 8, 0);
		circle(channel[0], rightmost, 10, Scalar(255), -1, 8, 0);
		circle(channel[0], topmost, 10, Scalar(255), -1, 8, 0);
		circle(channel[0], bottommost, 10, Scalar(255), -1, 8, 0);
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
		Rect myROI(leftmost.x - theoreticalLedSize,
				   topmost.y  - theoreticalLedSize,
				   (rightmost.x-leftmost.x)+2*theoreticalLedSize,
				   (bottommost.y-topmost.y)+2*theoreticalLedSize);
		cout << "Bounding rect " << myROI.x << " " << myROI.y << " " <<
				myROI.width << " " << myROI.height << endl;
		Mat result;
		merge(channel, 3, result);
		imshow( "Display window", result(myROI) );                   // Show our image inside it.
	} while ((key=waitKey(0)) != 'q');                                          // Wait for a keystroke in the window

	return 0;
}
