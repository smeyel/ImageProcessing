#ifndef __LUTDIFFCOLORFILTER_H
#define __LUTDIFFCOLORFILTER_H

#include <string>
#include <vector>
#include "LutColorFilter.h"

namespace smeyel
{
	/** LUT based difference color filter */
	class LutDiffColorFilter : public LutColorFilter
	{
		void Filter_All(cv::Mat &src0, cv::Mat &src1, cv::Mat &dst);
/*		void Filter_NoBoundingBox(cv::Mat &src0, cv::Mat &src1, cv::Mat &dst);
		void Filter_NoMatOutputNoMask(cv::Mat &src0, cv::Mat &src1); */
	protected:

	public:
		/** Constructor */
		LutDiffColorFilter();

		~LutDiffColorFilter();

		// HIDE THIS
		unsigned char ColorCodeToFind;

		/** Binary (0, 255) detection mask. Inidcates areas where differences ColorCodeToFind is located.
			@warning Only used if Filter is given dst and resultBoundingBoxes as well! Otherwise, omitted.
		*/

		cv::Mat *DetectionMask;

		/** Execute filter on an image
			@param src0	Source image 0
			@param src1	Source image 1
			@param dst	Destination image (may be NULL), contains the color codes
			@param boundingBoxes	Vector to collect detected areas (if not NULL)
		*/
		virtual void Filter(cv::Mat *src0, cv::Mat *src1, cv::Mat *dst, std::vector<cv::Rect> *resultBoundingBoxes = NULL);

		//void FilterRoI(cv::Mat &src0, cv::Mat &src1, cv::Rect &roi, cv::Mat &dst);
	};
}

#endif


