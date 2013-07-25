#ifndef __LUTCOLORFILTER_H
#define __LUTCOLORFILTER_H

#include "ColorFilter.h"

namespace smeyel
{
	/** LUT based color filter */
	class LutColorFilter : public ColorFilter
	{
		void Filter_All(cv::Mat &src, cv::Mat &dst);
		void Filter_NoBoundingBox(cv::Mat &src, cv::Mat &dst);
		void Filter_NoMatOutputNoMask(cv::Mat &src);
	protected:
		/** RGB Look-up table
			3 bit / RGB color layer -> 9 bit, 512 values. 0 means undefined color.
			Use SetLutItem to set. Take quantizing to 3 bits into account!
		*/
		uchar RgbLut[512];

		uchar *inverseLut;	// RGB order!

	public:
		/** Constructor */
		LutColorFilter();

		~LutColorFilter();

		/** Sets LUT to given colorcode for given RGB color.
			Used for runtime color LUT adjustments.
		*/
		void SetLutItem(uchar r, uchar g, uchar b, uchar colorCode);

		void InitLut(uchar colorCode);


		/** Color code to find. The bounding boxes will be created for areas of this color. */
		unsigned char ColorCodeToFind;

		/** Binary (0, 255) detection mask. Inidcates areas where ColorCodeToFind is located.
			@warning Only used if Filter is given dst and resultBoundingBoxes as well! Otherwise, omitted.
		*/

		cv::Mat *DetectionMask;

		/** Execute filter on an image
			@param src	Source image
			@param dst	Destination image (may be NULL), contains the color codes
			@param boundingBoxes	Vector to collect detected areas (if not NULL)
		*/
		virtual void Filter(cv::Mat *src, cv::Mat *dst, std::vector<cv::Rect> *resultBoundingBoxes = NULL);

		void FilterRoI(cv::Mat &src, cv::Rect &roi, cv::Mat &dst);


		/** Inverse LUT (for visualization) */
		void InverseLut(cv::Mat &src, cv::Mat &dst);
		void InitInverseLut(uchar r, uchar g, uchar b);
		void SetInverseLut(uchar colorCode, uchar r, uchar g, uchar b);

		// --- Aux helper functions
		/** @defgroup group1 Helper functions
		 *  These functions are used to gain deeper control over the LUT.
		 *  @{
		 */

		/** Returns the RGB values corresponding to a given LUT index
			@param	lutIdx	LUT Index
			@param	r	Red component (by reference)
			@param	g	Green component (by reference)
			@param	b	Blue component (by reference)
		*/
		void idx2rgb(unsigned int lutIdx, unsigned char &r, unsigned char &g, unsigned char &b);

		/** Calculates LUT index for given RGB value
			@param	r	Red component
			@param	g	Green component
			@param	b	Blue component
			@return		The LUT index
		*/
		unsigned int rgb2idx(unsigned char r, unsigned char g, unsigned char b);

		unsigned char idx2lutValue(unsigned int lutIdx);

		/** Converts an RGB value to another one which is really seen by the LUT after quantizing.
			@param	rOld	Original red component
			@param	gOld	Original green component
			@param	bOld	Original blue component
			@param	rNew	New red component (by reference)
			@param	gNew	New green component (by reference)
			@param	bNew	New blue component (by reference)
		*/
		void quantizeRgb(unsigned char rOld, unsigned char gOld, unsigned char bOld, unsigned char &rNew, unsigned char &gNew, unsigned char &bNew);

		/** Returns the LUT value for given RGB value.
			@param	r	Red component
			@param	g	Green component
			@param	b	Blue component
			@return		The value retrieved from the LUT.
		*/
		unsigned char rgb2lutValue(unsigned char r, unsigned char g, unsigned char b);

		/** Saves the LUT into a file.
			@param filename	The name of the file.
		*/
		void save(const char *filename);

		/** Loads the LUT from a file.
			@param filename	The name of the file.
		*/
		void load(const char *filename);

		/** Set LUT value for a given index.
			@param idx		LUT element index to change
			@param value	The new value.
		*/
		void setLutItemByIdx(unsigned int idx, unsigned char value);

		/** @} */
	};
}

#endif
