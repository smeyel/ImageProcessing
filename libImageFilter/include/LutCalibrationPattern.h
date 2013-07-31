#ifndef __LUTCALIBRATIONPATTERN_H
#define __LUTCALIBRATIONPATTERN_H

#include "LutColorFilter.h"
#include "Tetragon.h"

namespace smeyel
{
	class LutCalibrationPattern : public Tetragon
	{
		// Unit vectors from upper left corner.
		// Only calculated upon need, by getSubArea()
		Point2f unitHorizontal;
		Point2f unitVertical;

		// used by setFromCircleVector
		int findMinIdx(vector<Vec3f> marks, float coeffX, float coeffY, float offset);

		// used by findInImage
		void setFromCircleVector(vector<Vec3f> circles);

		void createColorCodeMask(Mat &dst);

	public:
		bool findInImage(Mat &src);

		void getSubArea(int row, int col, Tetragon &subArea);

		// helper to wrap getSubArea to result polyline instead of RectangularArea.
		void getPolylineForSubArea(int row, int col, Point *targetOf4Points);

		void updateLUT(LutColorFilter &lut, Mat &src);
	};
}

#endif
