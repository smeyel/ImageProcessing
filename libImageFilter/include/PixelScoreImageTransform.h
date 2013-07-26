#ifndef __PIXELSCOREIMAGETRANSFORM_H
#define __PIXELSCOREIMAGETRANSFORM_H

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)

#include "PixelScoreSource.h"

namespace smeyel
{
	class PixelScoreImageTransform
	{
		PixelScoreSource *source;
	public:
		PixelScoreImageTransform(PixelScoreSource *source);

		void TransformImage(cv::Mat &src, cv::Mat &dst);
	};
}

#endif
