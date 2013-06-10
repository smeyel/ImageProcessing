#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include "TimeSyncBeaconConverter.h"
#include "TimeSyncBeaconReader.h"
#include "SmartVector.h"
#include "CircleXYR.h"

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
	TimeSyncBeaconReader tsbr;
	char key=0;
	TimeSyncBeaconConverter tsbc(255,30,50,100,100);
	do {
		switch (key) {
		case 'I':
			tsbr.setThreshold(tsbr.getTreshold()+10);
			break;
		case 'D':
			tsbr.setThreshold(tsbr.getTreshold()-10);
			break;
		case 'i':
			tsbr.setThreshold(tsbr.getTreshold()+1);
			break;
		case 'd':
			tsbr.setThreshold(tsbr.getTreshold()-1);
			break;
		}
		cout << "thres = " << (unsigned)tsbr.getTreshold() << endl;
		tsbr.ProcessImage(sourceImage);

		tsbc.GetTimeUsFromBrightness(tsbr.davidArray);

		cout << "Bounding rect " << tsbr.getBoundingRect().x << " " << tsbr.getBoundingRect().y << " " <<
				tsbr.getBoundingRect().width << " " << tsbr.getBoundingRect().height << endl;
		Mat result;
		merge(tsbr.channel, 3, result);
		result += sourceImage;
		imshow( "Display window", result(tsbr.getBoundingRect()) );                   // Show our image inside it.
	} while ((key=waitKey(0)) != 'q');                                          // Wait for a keystroke in the window

	return 0;
}
