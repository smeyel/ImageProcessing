#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/opencv.hpp>			// For OPENCV_ASSERT

#include "ImageTransitionStat.h"

using namespace cv;
using namespace smeyel;

ImageTransitionStat::ImageTransitionStat(const unsigned int inputValueNumber, const unsigned int markovChainOrder, const char *runLengthTransformConfigFile)
	: TransitionStat(inputValueNumber, markovChainOrder, runLengthTransformConfigFile)
{
}


// Deprecated, use addImage(image,onRect) instead!
void ImageTransitionStat::addImage(Mat &image, bool isOn)
{
	// Assert for only 8UC1 output images
	OPENCV_ASSERT(image.type() == CV_8UC1,"feedImageIntoTransitionStat","Image type is not CV_8UC1");

	// Go along every pixel and do the following:
	for (int row=0; row<image.rows; row++)
	{
		// Calculate pointer to the beginning of the current row
		const uchar *ptr = (const uchar *)(image.data + row*image.step);
		
		// Sequences are restarted at the beginning of every image row.
		startNewSequence();
		// Go along every BGR colorspace pixel
		for (int col=0; col<image.cols; col++)
		{
			unsigned char value = *ptr++;

			addValue(value,isOn);
		}	// end for col
	}	// end for row
}

void ImageTransitionStat::addImage(Mat &image, Rect &onRect)
{
	// Assert for only 8UC1 output images
	OPENCV_ASSERT(image.type() == CV_8UC1,"feedImageIntoTransitionStat","Image type is not CV_8UC1");

	// Go along every pixel and do the following:
	for (int row=0; row<image.rows; row++)
	{
		// Calculate pointer to the beginning of the current row
		const uchar *ptr = (const uchar *)(image.data + row*image.step);
		
		// Sequences are restarted at the beginning of every image row.
		startNewSequence();
		// Go along every BGR colorspace pixel
		for (int col=0; col<image.cols; col++)
		{
			unsigned char value = *ptr++;

			bool isOn = onRect.contains(Point(col,row));
			addValue(value,isOn);
		}	// end for col
	}	// end for row
}

void ImageTransitionStat::addImageWithMask(cv::Mat &image, cv::Mat &mask)
{
	// Assert for only 8UC1 output images
	OPENCV_ASSERT(image.type() == CV_8UC1,"addImageWithMask","Image type is not CV_8UC1");
	OPENCV_ASSERT(mask.type() == CV_8UC1,"addImageWithMask","Mask type is not CV_8UC1");

	// Go along every pixel and do the following:
	for (int row=0; row<image.rows; row++)
	{
		// Calculate pointer to the beginning of the current row
		const uchar *imgPtr = (const uchar *)(image.data + row*image.step);
		const uchar *maskPtr = (const uchar *)(mask.data + row*mask.step);
		
		// Sequences are restarted at the beginning of every image row.
		startNewSequence();
		// Go along every BGR colorspace pixel
		for (int col=0; col<image.cols; col++)
		{
			unsigned char imageValue = *imgPtr++;
			unsigned char maskValue = *maskPtr++;
			bool isOn = (maskValue==255);

			addValue(imageValue,isOn);
		}	// end for col
	}	// end for row
}


// For debug purposes
void ImageTransitionStat::verboseScoreForImageLocation(Mat &src, Point pointToCheck)
{
	// Assert for only 8UC1 output images
	OPENCV_ASSERT(src.type() == CV_8UC1,"getVerboseScoreForImageLocation","src type is not CV_8UC1");

	// Calculate pointer to the beginning of the current row
	uchar *srcPtr = (uchar *)(src.data + pointToCheck.y*src.step);

	if (pointToCheck.x < markovChainOrder)
	{
		cout << "Point to check may be too close the the image border (x<markovChainOrder)" << endl;
	}

	uchar *ptr = NULL;
	for(unsigned int i=0; i<markovChainOrder; i++)
	{
		ptr = srcPtr+pointToCheck.x-markovChainOrder+i;
		lastValues[i]=*ptr;
		//*ptr = 0x00;	// Debug purposes
	}

	SequenceCounterTreeNode *node = NULL;

	cout << "Source and encoded buffer values:" << endl;
	showBufferContent("LastValues",lastValues,markovChainOrder);
	unsigned int length = this->runLengthTransform.transform();
	showBufferContent("LenEncoded",this->runLengthTransform.getInternalOutputBuffer(),length);

	node = counterTreeRoot->getNode(this->runLengthTransform.getInternalOutputBuffer(),length,true);
	if (node)
	{
		cout << "Node status=" << node->status << ", auxScore=" << (int)(node->auxScore);
		cout << ", counters(on/off): " << node->getCounter(COUNTERIDX_ON) << "/" << node->getCounter(COUNTERIDX_OFF) << endl;
	}
	else
	{
		cout << "No available node..." << endl;
	}
}
