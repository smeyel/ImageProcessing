#include "Logger.h"
#include "DetectionBoundingBoxCollector.h"

using namespace smeyel;

#define LOG_TAG	"DetectionBoundingBoxCollector"

DetectionBoundingBoxCollector::DetectionBoundingBoxCollector(BoundingBoxValidator *validator)
{
	// Pre-allocate space
	growingBoundingBoxes.reserve(100);
	initialBoundingBoxes.reserve(100);
	validatedBoundingBoxes.reserve(100);
	growingBoundingBoxes.clear();
	initialBoundingBoxes.clear();
	validatedBoundingBoxes.clear();

	boundingBoxValidator = validator;
}

void DetectionBoundingBoxCollector::StartNewFrame()
{
	growingBoundingBoxes.clear();
	initialBoundingBoxes.clear();
	validatedBoundingBoxes.clear();
}

void DetectionBoundingBoxCollector::RegisterDetection(int row, int colStart, int colEnd)
{
	// Search for detection rects with overlapping with given interval
	bool newRect=true;
	int growingBoundingBoxNum = growingBoundingBoxes.size();
	for(int i=0; i<growingBoundingBoxNum; i++)
	{
		if (!growingBoundingBoxes[i].isAlive)
		{
			continue;
		}
		// If there is an overlap (no sure gap on either sides)
		if (!(
			// detection is before the rect
			(growingBoundingBoxes[i].colMin > colEnd) 
			// detection is after the rect
			|| (growingBoundingBoxes[i].colMax < colStart) ))
		{
			// Update boundaries colMin and colMax
			if (growingBoundingBoxes[i].colMin > colStart)
			{
				growingBoundingBoxes[i].colMin = colStart;
			}
			if (growingBoundingBoxes[i].colMax < colEnd)
			{
				growingBoundingBoxes[i].colMax = colEnd;
			}

			// Register to keep this rect alive
			growingBoundingBoxes[i].isDetectedInCurrentRow = true;

			// Should not create a new detection for this
			newRect=false;
		}
	}

	if(newRect)
	{
		// Create new detection rect (find a place for it...)
		bool foundFreeDetectionRect = false;
		for(int i=0; i<growingBoundingBoxNum; i++)	// Find first !isAlive
		{
			if (!growingBoundingBoxes[i].isAlive)
			{
				growingBoundingBoxes[i].isAlive = true;
				growingBoundingBoxes[i].rowMin = row;
				growingBoundingBoxes[i].colMin = colStart;
				growingBoundingBoxes[i].colMax = colEnd;
				growingBoundingBoxes[i].isDetectedInCurrentRow = true;
				foundFreeDetectionRect = true;
				break;	// Stop searching
			}
		}
		// If there was no free detectionRect, do nothing...
		if (!foundFreeDetectionRect)
		{
			GrowingBoundingBox newGrowingBoundingBox;
			newGrowingBoundingBox.isAlive = true;
			newGrowingBoundingBox.rowMin = row;
			newGrowingBoundingBox.colMin = colStart;
			newGrowingBoundingBox.colMax = colEnd;
			newGrowingBoundingBox.isDetectedInCurrentRow = true;
			growingBoundingBoxes.push_back(newGrowingBoundingBox);
			// Send warning in this case!
			LogConfigTime::Logger::getInstance()->Log(LogConfigTime::Logger::LOGLEVEL_VERBOSE, LOG_TAG, "ColorFilter: has to extend detectionRects size\n");
		}
	}
}

void DetectionBoundingBoxCollector::FinishRow(int rowIdx)
{
	// Create bounding box from every discontinued detection rectangle
	// TODO: this will not create bounding boxes for detections touching the last row of the image!
	int growingBoundingBoxNum = growingBoundingBoxes.size();

	for(int i=0; i<growingBoundingBoxNum; i++)
	{
		if (!growingBoundingBoxes[i].isAlive)
		{
			continue;
		}
		// Ends in current row or not?
		if (!growingBoundingBoxes[i].isDetectedInCurrentRow)
		{
			// Not seen anymore, store as initial bounding box
			initialBoundingBoxes.push_back(growingBoundingBoxes[i].getRect(rowIdx));

			// Deactivate detection rect
			growingBoundingBoxes[i].isAlive = false;
		}
		else
		{
			// Reset detection status for next row
			growingBoundingBoxes[i].isDetectedInCurrentRow = false;
		}
	}
}

void DetectionBoundingBoxCollector::RegisterBoundingBox(cv::Rect &rect)
{
	initialBoundingBoxes.push_back(rect);
}

void DetectionBoundingBoxCollector::ValidateBoundingBoxes()
{
	// Simply copy qualifying bounding boxes
	// TODO: later add merges, overlap checks etc.
	for (unsigned int i=0; i<initialBoundingBoxes.size(); i++)
	{
		if (this->boundingBoxValidator->Validate(initialBoundingBoxes[i]))
			validatedBoundingBoxes.push_back(initialBoundingBoxes[i]);
	}
}

void DetectionBoundingBoxCollector::ShowBoundingBoxes(cv::Mat &img, cv::Scalar color, bool drawInitials)
{
	std::vector<cv::Rect> *vector = drawInitials ? &this->initialBoundingBoxes : &this->validatedBoundingBoxes;

	for(unsigned int i=0; i<vector->size(); i++)
	{
		rectangle(img,(*vector)[i],color);
	}
}
