#include <opencv2/opencv.hpp>			// For OPENCV_ASSERT

#include "PixelScoreImageTransform.h"

using namespace smeyel;

PixelScoreImageTransform::PixelScoreImageTransform(PixelScoreSource *source)
{
	this->source = source;
}

void PixelScoreImageTransform::TransformImage(cv::Mat &src, cv::Mat &dst)
{
	// Assert for only 8UC1 output images
	OPENCV_ASSERT(src.type() == CV_8UC1,"getScoreMaskForImage","src type is not CV_8UC1");
	OPENCV_ASSERT(dst.type() == CV_8UC1,"getScoreMaskForImage","dst type is not CV_8UC1");
	OPENCV_ASSERT(src.rows=dst.rows,"getScoreMaskForImage","src and dst size does not match");
	OPENCV_ASSERT(src.cols=dst.cols,"getScoreMaskForImage","src and dst size does not match");

	// Go along every pixel and do the following:
	for (int row=0; row<src.rows; row++)
	{
		// Calculate pointer to the beginning of the current row
		const uchar *srcPtr = (const uchar *)(src.data + row*src.step);
		uchar *dstPtr = (uchar *)(dst.data + row*dst.step);

		// Sequences are restarted at the beginning of every image row.
		source->startNewSequence();
		// Go along every BGR colorspace pixel
		for (int col=0; col<src.cols; col++)
		{
			unsigned char value = *srcPtr++;

			*dstPtr = source->getScoreForValue(value);
			//*dstPtr = getScoreForRunLengthQuantizedValue(value);
			dstPtr++;
		}	// end for col
	}	// end for row
}
