#ifndef __COLORFILTER_H
#define __COLORFILTER_H
#include <stdlib.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>

#include "DetectionBoundingBoxCollector.h"

namespace smeyel
{
	/** Abstract base class for color filters */
	class ColorFilter
	{
	public:
		DetectionBoundingBoxCollector *defaultCollector;

		DetectionBoundingBoxCollector *collector;

	protected:
		void ConsolidateBoundingBoxes()
		{
			collector->ValidateBoundingBoxes();
		}

		void StartNewFrame()
		{
			collector->StartNewFrame();
		}

		void RegisterDetection(int row, int colStart, int colEnd)
		{
			collector->RegisterDetection(row, colStart, colEnd);
		}

		void FinishRow(int rowIdx)
		{
			collector->FinishRow(rowIdx);
		}

	public:

		/** Constructor */
		ColorFilter();

		~ColorFilter();

		/** Execute filter on an image
			@param src	Source image
			@param dst	Destination image (may be NULL)
			@param boundingBoxes	Vector to collect detected areas (if applicable and not NULL)
		*/
		virtual void Filter(cv::Mat *src, cv::Mat *dst, std::vector<cv::Rect> *resultBoundingBoxes = NULL) = 0;

		void ShowBoundingBoxes(cv::Mat &img, cv::Scalar color);
	};





}



#endif