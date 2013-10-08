#include <fstream>
#include "LutDiffColorFilter.h"

using namespace smeyel;
using namespace cv;

#define LASTDETECTIONCOL_NONE -1

LutDiffColorFilter::LutDiffColorFilter()
{
}

LutDiffColorFilter::~LutDiffColorFilter()
{
}

/*void LutDiffColorFilter::FilterRoI(Mat &src0, Mat &src1, Rect &roi, Mat &dst)
{
	assert(src0.type() == CV_8UC3);
	assert(src1.type() == CV_8UC3);
	assert(dst.type() == CV_8UC1);

	Mat src0Roi(src0,roi);
	Mat src1Roi(src1,roi);
	Mat dstRoi(dst,roi);

	Filter_NoBoundingBox(src0Roi,src1Roi,dstRoi);
}*/

void LutDiffColorFilter::Filter(Mat *src0, Mat *src1, Mat *dst, std::vector<Rect> *resultBoundingBoxes)
{
	// Assert for only 8UC3 input images (BGR)
	assert(src0->type() == CV_8UC3);
	assert(src1->type() == CV_8UC3);

	if (dst!=NULL && resultBoundingBoxes!=NULL)
	{
		Filter_All(*src0,*src1,*dst);
		// Copy bounding boxes from internal vector
		*resultBoundingBoxes = collector->getCopyValidBoundingBoxes();
	}
	else if (dst!=NULL && resultBoundingBoxes==NULL)
	{
		Filter_NoBoundingBox(*src0,*src1,*dst);
	}
/*	else if (dst==NULL && resultBoundingBoxes!=NULL)
	{
		Filter_NoMatOutputNoMask(*src0,*src1);
		// Copy bounding boxes from internal vector
		*resultBoundingBoxes = collector->getCopyValidBoundingBoxes();
	} */
}

void LutDiffColorFilter::Filter_All(Mat &src0, Mat &src1, cv::Mat &dst)
{
	// Assert for only 8UC1 output images	assert(dst.type() == CV_8UC1);
	// Assert dst has same size as src
	assert(src0.cols == dst.cols);
	assert(src0.rows == dst.rows);
	assert(src1.cols == dst.cols);
	assert(src1.rows == dst.rows);

	// Assert destination image type is CV_8UC1
	assert(DetectionMask->type() == CV_8UC1);
	// Assert mask size
	assert(DetectionMask->cols == dst.cols);
	assert(DetectionMask->rows == dst.rows);

	uchar colorCode0;
	uchar colorCode1;
	uchar detectColorCode = this->ColorCodeToFind; // Stored for faster access

	int lastDetectionCol = LASTDETECTIONCOL_NONE;
	int continuousDetectionStartCol = -1;

	// Init processing of a new frame
	StartNewFrame();

	// Go along every pixel and do the following:
	for (int row=0; row<src0.rows; row++)
	{
		// Calculate pointer to the beginning of the current row
		const uchar *ptr0 = (const uchar *)(src0.data + row*src0.step);
		const uchar *ptr1 = (const uchar *)(src1.data + row*src1.step);
		// Result pointer
		uchar *resultPtr = (uchar *)(dst.data + row*dst.step);
		// Update mask data pointers
		uchar *detectionMaskPtr = (uchar *)(DetectionMask->data + row*DetectionMask->step);

		// Reset tracking of continuous detections
		lastDetectionCol = LASTDETECTIONCOL_NONE;
		continuousDetectionStartCol = -1;

		// Go along every BGR colorspace pixel
		for (int col=0; col<src0.cols; col++)
		{
			uchar B = *ptr0++;
			uchar G = *ptr0++;
			uchar R = *ptr0++;
			unsigned int idxR = R >> 5;
			unsigned int idxG = G >> 5;
			unsigned int idxB = B >> 5;
			unsigned int idx = (idxR << 6) | (idxG << 3) | idxB;
			colorCode0 = RgbLut[idx];

			B = *ptr1++;
			G = *ptr1++;
			R = *ptr1++;
			idxR = R >> 5;
			idxG = G >> 5;
			idxB = B >> 5;
			idx = (idxR << 6) | (idxG << 3) | idxB;
			colorCode1 = RgbLut[idx];

			uchar codeDiff = (colorCode0 != colorCode1) ? 255 : 0;

			*resultPtr++ = codeDiff;

			// Handle masks (2 masks)
			*detectionMaskPtr = codeDiff;
			
			if (codeDiff)
			{
				// Handle detection collection
				if (lastDetectionCol == LASTDETECTIONCOL_NONE)	// Starting first detection in this column
				{
					// Start new continuous detection
					continuousDetectionStartCol = col;
				}
				else if (lastDetectionCol < col-1)	// There was a gap since last detection
				{
					// Register last continuous detection
					RegisterDetection(row,continuousDetectionStartCol,lastDetectionCol);

					// Start new continuous detection
					continuousDetectionStartCol = col;
				}
				lastDetectionCol = col;
			}

			detectionMaskPtr++;
		}	// end for col
		// Starting new row (if any...)
		if (lastDetectionCol != LASTDETECTIONCOL_NONE)
		{
			// There is still an unregistered detection left
			RegisterDetection(row,continuousDetectionStartCol,lastDetectionCol);
		}

		FinishRow(row);
	}	// end for row
}

void LutDiffColorFilter::Filter_NoBoundingBox(Mat &src0, Mat &src1, cv::Mat &dst)
{
	// Assert for only 8UC1 output images	assert(dst.type() == CV_8UC1);
	// Assert dst has same size as src
	assert(src0.cols == dst.cols);
	assert(src0.rows == dst.rows);
	assert(src1.cols == dst.cols);
	assert(src1.rows == dst.rows);

	uchar colorCode0;
	uchar colorCode1;
	uchar detectColorCode = this->ColorCodeToFind; // Stored for faster access

	// Go along every pixel and do the following:
	for (int row=0; row<src0.rows; row++)
	{
		// Calculate pointer to the beginning of the current row
		const uchar *ptr0 = (const uchar *)(src0.data + row*src0.step);
		const uchar *ptr1 = (const uchar *)(src1.data + row*src1.step);
		// Result pointer
		uchar *resultPtr = (uchar *)(dst.data + row*dst.step);

		// Go along every BGR colorspace pixel
		for (int col=0; col<src0.cols; col++)
		{
			uchar B = *ptr0++;
			uchar G = *ptr0++;
			uchar R = *ptr0++;
			unsigned int idxR = R >> 5;
			unsigned int idxG = G >> 5;
			unsigned int idxB = B >> 5;
			unsigned int idx = (idxR << 6) | (idxG << 3) | idxB;
			colorCode0 = RgbLut[idx];

			B = *ptr1++;
			G = *ptr1++;
			R = *ptr1++;
			idxR = R >> 5;
			idxG = G >> 5;
			idxB = B >> 5;
			idx = (idxR << 6) | (idxG << 3) | idxB;
			colorCode1 = RgbLut[idx];

			uchar codeDiff = (colorCode0 != colorCode1) ? 255 : 0;

			*resultPtr++ = codeDiff;
		}	// end for col
	}	// end for row
}

/*void LutDiffColorFilter::Filter_NoMatOutputNoMask(Mat &src0, Mat &src1)
{
	// Bounding boxes only...

	uchar colorCode;
	uchar detectColorCode = this->ColorCodeToFind; // Stored for faster access

	int lastDetectionCol = LASTDETECTIONCOL_NONE;
	int continuousDetectionStartCol = -1;

	// Init processing of a new frame
	StartNewFrame();

	// Go along every pixel and do the following:
	for (int row=0; row<src0.rows; row++)
	{
		// Calculate pointer to the beginning of the current row
		const uchar *ptr = (const uchar *)(src.data + row*src.step);

		// Reset tracking of continuous detections
		lastDetectionCol = LASTDETECTIONCOL_NONE;
		continuousDetectionStartCol = -1;

		// Go along every BGR colorspace pixel
		for (int col=0; col<src.cols; col++)
		{
			uchar B = *ptr++;
			uchar G = *ptr++;
			uchar R = *ptr++;
			unsigned int idxR = R >> 5;
			unsigned int idxG = G >> 5;
			unsigned int idxB = B >> 5;
			unsigned int idx = (idxR << 6) | (idxG << 3) | idxB;
			colorCode = RgbLut[idx];

			// Handle masks (2 masks)
			uchar isDetectColor = (colorCode==detectColorCode)?255:0;
			
			if (isDetectColor)
			{
				// Handle detection collection
				if (lastDetectionCol == LASTDETECTIONCOL_NONE)	// Starting first detection in this column
				{
					// Start new continuous detection
					continuousDetectionStartCol = col;
				}
				else if (lastDetectionCol < col-1)	// There was a gap since last detection
				{
					// Register last continuous detection
					RegisterDetection(row,continuousDetectionStartCol,lastDetectionCol);

					// Start new continuous detection
					continuousDetectionStartCol = col;
				}
				lastDetectionCol = col;
			}
		}	// end for col
		// Starting new row (if any...)
		if (lastDetectionCol != LASTDETECTIONCOL_NONE)
		{
			// There is still an unregistered detection left
			RegisterDetection(row,continuousDetectionStartCol,lastDetectionCol);
		}

		FinishRow(row);
	}	// end for row
}
*/