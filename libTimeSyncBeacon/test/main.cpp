#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    if( argc != 2)
    {
     cout <<" Usage: " << argv[0] << " display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }

    Mat image;
    image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    Mat channel[3];
    split(image, channel);

    MatIterator_<uchar> it, end;
    for( it = channel[2].begin<uchar>(), end = channel[2].end<uchar>(); it != end; ++it)
    {
        (*it)=((*it)>150)?255:0;
    }
    namedWindow( "Display window", CV_WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window", channel[2] );                   // Show our image inside it.

    while (waitKey(0) != 'q');                                          // Wait for a keystroke in the window
    return 0;
}
