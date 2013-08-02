#ifndef __SIMPLEBOUNDINGBOXVALIDATOR_H
#define __SIMPLEBOUNDINGBOXVALIDATOR_H
#include "BoundingBoxValidator.h"

namespace smeyel
{
	class SimpleBoundingBoxValidator : public BoundingBoxValidator
	{
	public:
		int minWidht, maxWidth;
		int minHeight, maxHeight;
		float minRatio, maxRatio;	// width / height

		SimpleBoundingBoxValidator()
		{
			// Set defaults
			minWidht = 5;
			maxWidth = 300;
			minHeight = 5;
			maxHeight = 300;
			minRatio = 0.2F;
			maxRatio = 5.0F;
		}

		virtual bool Validate(cv::Rect rect)
		{
			if (rect.width < minWidht || rect.width > maxWidth)
				return false;
			if (rect.height < minHeight || rect.height > maxHeight)
				return false;
			float ratio = (float)rect.width / (float)rect.height;
			if (ratio < minRatio || ratio > maxRatio)
				return false;
			return true;
		}
	};
}

#endif
