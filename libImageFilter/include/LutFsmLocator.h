#ifndef __LUTFSMLOCATOR_H
#define __LUTFSMLOCATOR_H
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <stdlib.h>	// for vector

#include "FsmColorFilter.h"
#include "FsmBuilder.h"

namespace smeyel
{
	/** LutFsmLocator may be used to find detection areas using FSM, and then generate a LUT image
		inside the detection bounding boxes.
		(For performance reasons, FSM execution uses the LUT, but does not create an image of it.
		That is only done for the detection areas.)
		
		For monitoring purposes, the following functions are provided:
		- verboseFsmState: visualize FSM state for every pixel
		- verboseLutImage: visualize LUT results
		- cleanVerboseImage: if false, verboseLutImage is not reset before new frames.
			(This leaves currently untouched areas preserve previous content.)
		- showBoundingBoxesOnSrc: shows bounding boxes on the source image
		- overrideFullLut: perform full LUT for every pixel of LutImage,
			not only inside the boundig boxes.

		@warning: state and lut visualization (InverseLUT) share the same colorcode space!
	*/
	class LutFsmLocator : protected FsmColorFilter
	{
	private:
		// Used internally by processImage. Recycled for successive frames.
		std::vector<cv::Rect> bbVector;

	protected:
		/** Applies a given FsmBuilder */
		void useFsmBuilder(FsmBuilder &builder, unsigned int initialState, unsigned int minStateIdToSave, unsigned int minStateIdToCommit);
		/** Applies given bounding box size check rules */
		void useBoundingBoxCheckParams(ColorFilter::BoundingBoxCheckParams params);

		/** Perform any kind of modification to the list of bounding boxes */
		virtual void processBoundingBoxes(std::vector<cv::Rect> &boundingBoxes) { }

		/** Perform validation of a single bounding box using LUT results on its area
		*/
		virtual void processSingleBoundingBox(cv::Rect &boundingBox, cv::Mat &lutImage, cv::Mat &originalImage) { }
	public:
		// Used by processImage to store temporal FSM state image before InverseLUT-ing it.
		cv::Mat *fsmStateImage;

		// Used by processImage, LUT image for areas of bounding boxes
		// Created based on first processed frame.
		cv::Mat *lutImage;

		cv::Mat *verboseFsmState;
		cv::Mat *verboseLutImage;

		/** If true, the verboseFsmState image is cleared before every frame.
			Otherwise, remainings of (now untouched) areas from previous frames may appear.
		*/
		bool cleanVerboseImage;
		bool showBoundingBoxesOnSrc;
		bool overrideFullLut;

		LutFsmLocator();

		~LutFsmLocator();

		/** Processes the image. Calls internal processing steps which can be overridden in derived classes.
			Results may be saved in attributes of derived classes.

			Results of the function:
			- bbVector contains the valid bounding boxes
			- if showBoundingBoxesOnSrc, src shows the valid bounding boxes and their number
			- if verboseFsmState is not NULL, it contains the LUT-ed (+InverseLUT-ed) image
				processed only for the areas of bounding boxes.
				If cleanVerboseImage is false, it is not cleared after the frames.
		*/
		void processImage(cv::Mat &src);
	};
}
#endif