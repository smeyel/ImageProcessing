#ifndef __IMAGETRANSITIONSTAT_H
#define __IMAGETRANSITIONSTAT_H
#include "TransitionStat.h"

namespace smeyel
{
	class ImageTransitionStat : public TransitionStat
	{
	public:
		ImageTransitionStat(const unsigned int inputValueNumber, const unsigned int markovChainOrder, const char *runLengthTransformConfigFile = NULL)
			: TransitionStat(inputValueNumber, markovChainOrder, runLengthTransformConfigFile)
		{
		}

		/** Feeds a CV_81C1 (like LUT) image using addValue pixel-by-pixel. */
		void addImage(cv::Mat &image, bool isOn);
		void addImage(cv::Mat &image, cv::Rect &onRect);
		void addImageWithMask(cv::Mat &image, cv::Mat &mask);


		/** Retrieves the score with getScoreForValue() for every pixel. */
		void getScoreMaskForImage(cv::Mat &src, cv::Mat &dst);

		void verboseScoreForImageLocation(cv::Mat &src, cv::Point pointToCheck);
	};
}

#endif
