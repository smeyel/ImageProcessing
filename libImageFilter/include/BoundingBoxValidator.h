#ifndef __BOUNDINGBOXVALIDATOR_H
#define __BOUNDINGBOXVALIDATOR_H
#include <opencv2/core/core.hpp>

namespace smeyel
{
	class BoundingBoxValidator
	{
	public:
		/** Returns true for valid bounding boxes. */
		virtual bool Validate(cv::Rect rect) = 0;
	};
}

#endif
