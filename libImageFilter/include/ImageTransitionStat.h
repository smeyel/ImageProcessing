#ifndef __IMAGETRANSITIONSTAT_H
#define __IMAGETRANSITIONSTAT_H
#include "TransitionStat.h"

namespace smeyel
{
	/** Extends TransitionStat with functions operating on whole images.
	*/
	class ImageTransitionStat : public TransitionStat
	{
	public:
		/** Constructor.
			@param inputValueNumber	Number of possible input values. If you are using an image resulting from a LutColorFilter, this is the maximal number of LUT values (colorcodes).
			@param markovChainOrder The number of pixels the statistics takes into account (irrespective on possible run length transformations).
			@param runLengthTransformConfigFile	Name of the config file for the internal RunLengthTransform class.
		*/
		ImageTransitionStat(const unsigned int inputValueNumber, const unsigned int markovChainOrder, const char *runLengthTransformConfigFile = NULL);

		/** Functions feed a CV_81C1 (like LUT) image using addValue pixel-by-pixel.
			@{
		*/
		void addImage(cv::Mat &image, bool isOn);
		void addImage(cv::Mat &image, cv::Rect &onRect);
		void addImageWithMask(cv::Mat &image, cv::Mat &mask);
		/** @} */

		/** Retrieves the score with getScoreForValue() for every pixel. */
		//void getScoreMaskForImage(cv::Mat &src, cv::Mat &dst);

		/** Verboses the score calculation for a given image location to the standard output. */
		void verboseScoreForImageLocation(cv::Mat &src, cv::Point pointToCheck);
	};
}

#endif
