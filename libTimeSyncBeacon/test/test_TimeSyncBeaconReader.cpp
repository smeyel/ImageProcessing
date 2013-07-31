#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "TimeSyncBeacon.h" 			//Needed for single run

#include "TimeSyncBeaconConverter.h" 	//Needed for interactive mode
#include "TimeSyncBeaconReader.h"		//Needed for single run mode


using namespace cv;
using namespace std;

Mat sourceImage;

void interactiveMode()
{
	namedWindow( "Display window", CV_WINDOW_AUTOSIZE );// Create a window for display.
	TimeSyncBeaconReader tsbr;
	tsbr.setThreshold(100);
	char key=0;
	TimeSyncBeaconConverter tsbc(255,10,10,100,10);
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
		tsbr.GenerateImage();
		merge(tsbr.channel, 3, result);
		result += sourceImage;
		imshow( "Display window", result(tsbr.getBoundingRect()) );                   // Show our image inside it.
		cout << "Press q to quit" << endl;
	} while ((key=waitKey(0)) != 'q');                                          // Wait for a keystroke in the window
}

void singleRun()
{
	TimeSyncBeacon tsb;
	cout << "TimeSyncBeacon time " << tsb.GetTimeUsFromImage(sourceImage) << endl;

}

int main( int argc, char** argv )
{
	if( argc < 2)
	{
		cout <<" Usage: " << argv[0] << " infile [--interactive]" << endl;
		return -1;
	}

	sourceImage = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

	if(! sourceImage.data )                              // Check for invalid input
	{
		cout <<  "Could not open or find the image" << std::endl ;
		return -1;
	}

	if ((argc==3) && (strcmp(argv[2], "--interactive") == 0))
		interactiveMode();
	else
		singleRun();

	return 0;
}
