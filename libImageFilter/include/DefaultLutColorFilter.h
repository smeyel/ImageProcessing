#ifndef __DEFAULTLUTCOLORFILTER_H
#define __DEFAULTLUTCOLORFILTER_H
#include "LutColorFilter.h"

#include "DefaultColorCodes.h"

using namespace smeyel;

/** A LutColorFilter using the color codes in DefaultColorCodes.h, contains their names and
	has an approprietly initialized inverse lut to restore those colors.

	Usage:
	- Instantiate, that sets up a default LUT.
		lutColorFilter = new DefaultLutColorFilter();
	- You can use load() to load another LUT from file.

	If you want to use the calibration pattern:

	- Instantiate a LutCalibrationPattern
		LutCalibrationPattern area;
	- Call findInImage() to locate the pattern in an image (src). This returns true if a pattern was found.
		if (!area.findInImage(src))
		{
			return;	// Not found...
		}
	- Optionally, you can draw it on the image
		area.draw(src,Scalar(100,255,100));
	- If you want, you can compensate the perspective transformation (also possible slight rotations)
		Mat normalizedImage;
		area.compensatePerspectiveTransform(src,normalizedImage);
	- Optionally, you can draw all subareas of the pattern
		Tetragon subarea;
		area.getSubArea(row,col,subarea);
		subarea.draw(verboseImage,Scalar(255,100,100));
	- Call updateLUT() to update the LUT based on the pattern in the image.
		area.updateLUT(*lutColorFilter,normalizedImage);
*/
class DefaultLutColorFilter : public LutColorFilter
{
public:
	DefaultLutColorFilter();
	void init();
};

#endif
