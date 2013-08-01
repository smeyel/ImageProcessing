#ifndef __DETECTIONBOUNDINGBOXCOLLECTOR_H
#define __DETECTIONBOUNDINGBOXCOLLECTOR_H
#include <vector>
#include "BoundingBoxValidator.h"

namespace smeyel
{
	/** It may be used together with a detection mask to handle arbitrary shaped detection areas. */
	class DetectionBoundingBoxCollector
	{
	private:
		/** Represents a still evolving bounding box for internal use only.
			Used to keep track of the blobs to
			finally get the location and size of the blob.
		*/
		class GrowingBoundingBox
		{
		public:
			int rowMin;	// First row of detection, set at first encounter
			int colMin, colMax;	// horizontal boundaries, kept up-to-date
			bool isAlive;	// is this struct used at all?
			bool isDetectedInCurrentRow;	// Was there a detection in the current row?
			cv::Rect getRect(int lastDetectionRow)
			{
				cv::Rect rect;
				rect.x = colMin;
				rect.y = rowMin;
				rect.width = colMax-colMin;
				rect.height = lastDetectionRow-rowMin;
				return rect;
			}
		};

		/** Used during image processing to evolve the growing bounding boxes. */
		std::vector<GrowingBoundingBox> growingBoundingBoxes;

		/** During detection, closed bounding boxes are saved here.
			There may still be overlappings. */
		std::vector<cv::Rect> initialBoundingBoxes;

		/** Validated bounding boxes are saved here.
			@warning: List is only avaliable until the next call to StartNewFrame()!
		*/
		std::vector<cv::Rect> validatedBoundingBoxes;

		BoundingBoxValidator *boundingBoxValidator;

	public:
		/** Constructor */
		DetectionBoundingBoxCollector(BoundingBoxValidator *validator);

		/** Inits processing of a new frame.
			Clears bounding box containers.	*/
		void StartNewFrame();

		/** Indicates the detection between pixels from colStart and colEnd in the current pixel row.
			@param row	Index if the current row
			@param colStart	Column where the detection starts
			@param colEnd	Column where the detection ends
			
			Works on growingBoundingBoxes.
		*/
		void RegisterDetection(int row, int colStart, int colEnd);

		/** Post-processes the detections of the current row.

			Works on growingBoundingBoxes and closed bounding boxes
			are copied into initialBoundingBoxes.
		*/
		void FinishRow(int rowIdx);

		/** May be used to add a bounding box directly.
			Works on initialBoundingBoxes.
		*/
		void RegisterBoundingBox(cv::Rect &rect);

		/** Every bounding box in initialBoundingBoxes conforming boundingBoxValidator
			is copied into validatedBoundingBoxes. */
		void ValidateBoundingBoxes();

		std::vector<cv::Rect> getCopyValidBoundingBoxes()
		{
			return this->validatedBoundingBoxes;
		}

		/** Draws rectangles in validatedBoundingBoxes or
			initialBoundingBoxes depending on drawInitials. */
		void ShowBoundingBoxes(cv::Mat &img, cv::Scalar color, bool drawInitials=false);
	};
}

#endif
