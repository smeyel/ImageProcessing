#ifndef __PIXELSCOREIMAGETRANSFORM_H
#define __PIXELSCOREIMAGETRANSFORM_H

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)

#include "PixelScoreSource.h"

namespace smeyel
{
	/** Uses a PixelScoreSource to assign its score to every pixel of an image.
	*/
	class PixelScoreImageTransform
	{
		PixelScoreSource *source;
	public:
		PixelScoreImageTransform(PixelScoreSource *source);

		void TransformImage(cv::Mat &src, cv::Mat &dst);
	};
}

#endif
