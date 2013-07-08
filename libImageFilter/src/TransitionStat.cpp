#include "TransitionStat.h"

using namespace smeyel;
using namespace cv;

TransitionStat::TransitionStat(const unsigned int inputValueNumber, const unsigned int markovChainOrder, const unsigned int initialValue)
{
	counterTreeRoot = new SequenceCounterTreeNode(inputValueNumber);
	lastValues = new unsigned int[markovChainOrder];
	this->inputValueNumber = inputValueNumber;
	this->markovChainOrder = markovChainOrder;
	this->initialValue = initialValue;

	startNewSequence();

	lastValue = -1;
	lastIsTargetArea = false;
	runLength = 0;
	lastScore = 0;

	trainMinPrecision=0.9F;
	trainMinSampleNum=50;

}

TransitionStat::~TransitionStat()
{
	delete counterTreeRoot;
	delete lastValues;
	counterTreeRoot = NULL;
	lastValues = NULL;
}

void TransitionStat::startNewSequence()
{
	for(unsigned int i=0; i<markovChainOrder; i++)
		lastValues[i]=initialValue;
	valueNumberSinceSequenceStart=0;
}

/**	Adds a new value to the sequence.
	Call this function with every input sequence element to create the statistic.
	@param	inputValue	The value of the next element of the sequence the statistic is based on.
	@param	isTargetArea	Shows whether the current element is inside a target area or not.
*/
void TransitionStat::addValue(const unsigned int inputValue, const bool isTargetArea)
{
	OPENCV_ASSERT(inputValue<inputValueNumber,"TransitionStat.addValue","value > max!");
	valueNumberSinceSequenceStart++;
	// Update history (lastValues)
	for(unsigned int i=0; i<markovChainOrder-1; i++)
		lastValues[i]=lastValues[i+1];
	lastValues[markovChainOrder-1] = inputValue;

	// Increment the counter only if initial values are already shifted out of the lastValues array.
	if (valueNumberSinceSequenceStart>=markovChainOrder)
	{
		SequenceCounterTreeNode *node = NULL;
		node = counterTreeRoot->getNode(lastValues,markovChainOrder,true);

		// Increment respective counter
		node->incrementCounter(isTargetArea ? COUNTERIDX_ON : COUNTERIDX_OFF);
	}
}

unsigned char TransitionStat::getScoreForValue(const unsigned int inputValue)
{
	OPENCV_ASSERT(inputValue<inputValueNumber,"TransitionStat.getScoreForValue","value > max!");
	// Update history (lastValues)
	for(unsigned int i=0; i<markovChainOrder-1; i++)
		lastValues[i]=lastValues[i+1];
	lastValues[markovChainOrder-1] = inputValue;

	SequenceCounterTreeNode *node = NULL;
	node = counterTreeRoot->getNode(lastValues,markovChainOrder,false);

	// Increment respective counter
	if (node)
	{
		return node->auxScore;
	}
	return 0;
}

void TransitionStat::addImage(Mat &image, bool isOn)
{
	// Assert for only 8UC1 output images
	OPENCV_ASSERT(image.type() == CV_8UC1,"feedImageIntoTransitionStat","Image type is not CV_8UC1");

	startNewSequence();

	// Go along every pixel and do the following:
	for (int row=0; row<image.rows; row++)
	{
		// Calculate pointer to the beginning of the current row
		const uchar *ptr = (const uchar *)(image.data + row*image.step);

		// Go along every BGR colorspace pixel
		for (int col=0; col<image.cols; col++)
		{
			unsigned char value = *ptr++;

			addValue(value,isOn);
			//addRunLengthQuantizedValue(value,isOn);
		}	// end for col
	}	// end for row
}

void TransitionStat::getScoreMaskForImage(Mat &src, Mat &dst)
{
	// Assert for only 8UC1 output images
	OPENCV_ASSERT(src.type() == CV_8UC1,"getScoreMaskForImage","src type is not CV_8UC1");
	OPENCV_ASSERT(dst.type() == CV_8UC1,"getScoreMaskForImage","dst type is not CV_8UC1");
	OPENCV_ASSERT(src.rows=dst.rows,"getScoreMaskForImage","src and dst size does not match");
	OPENCV_ASSERT(src.cols=dst.cols,"getScoreMaskForImage","src and dst size does not match");

	// Go along every pixel and do the following:
	for (int row=0; row<src.rows; row++)
	{
		// Calculate pointer to the beginning of the current row
		const uchar *srcPtr = (const uchar *)(src.data + row*src.step);
		uchar *dstPtr = (uchar *)(dst.data + row*dst.step);

		// Go along every BGR colorspace pixel
		for (int col=0; col<src.cols; col++)
		{
			unsigned char value = *srcPtr++;

			//*dstPtr = stat->getScoreForValue(value);
			*dstPtr = getScoreForRunLengthQuantizedValue(value);
			dstPtr++;
		}	// end for col
	}	// end for row
}

void TransitionStat::addRunLengthQuantizedValue(unsigned char value, bool isTargetArea)
{
	if (value==lastValue)
	{
		runLength++;
		lastIsTargetArea = isTargetArea;
	}
	else
	{
		int quantizedRunLength = 0;
		if (runLength>=5)	quantizedRunLength = 1;
		if (runLength>=10)	quantizedRunLength = 2;
		if (runLength>=20)	quantizedRunLength = 3;
		for (int i=0; i<quantizedRunLength; i++)
			addValue(lastValue,lastIsTargetArea);
		lastValue = value;
		runLength = 1;
		lastIsTargetArea = isTargetArea;
	}
}

unsigned char TransitionStat::getScoreForRunLengthQuantizedValue(unsigned char value)
{
	if (value==lastValue)
	{
		runLength++;
	}
	else
	{
		int quantizedRunLength = 0;
		if (runLength>=5)	quantizedRunLength = 1;
		if (runLength>=10)	quantizedRunLength = 2;
		if (runLength>=20)	quantizedRunLength = 3;
		for (int i=0; i<quantizedRunLength-1; i++)	// Fetch n-1 values...
			getScoreForValue(value);
		unsigned char score = 0;	// Default for initial cases...
		if (quantizedRunLength-1>0)
		{
			score = getScoreForValue(value);
		}
		lastScore = score;
		lastValue = value;
		runLength = 1;
	}
	return lastScore;
}

void TransitionStat::checkNode(SequenceCounterTreeNode *node, float sumOn, float sumOff, int maxInputValue, notifycallbackPtr callback)
{
	int onNum = node->getCounter(COUNTERIDX_ON);
	int offNum = node->getCounter(COUNTERIDX_OFF);
	int sumNum = onNum+offNum;
	float onRate = (float)onNum / (float)sumOn;
	float offRate = (float)offNum / (float)sumOff;
	float precision = onRate / (onRate+offRate);
	if (precision >= trainMinPrecision && sumNum>=trainMinSampleNum )
	{
		(*callback)(node, precision);
	}
	// Continue on children
	for (int i=0; i<=maxInputValue; i++)
	{
		SequenceCounterTreeNode *child = node->getChildNode(i,false);
		if (child)
		{
			checkNode(child,sumOn,sumOff,maxInputValue,callback);
		}
	}
}

void TransitionStat::findClassifierSequences(notifycallbackPtr callback)
{
	// Recursively go along every sequence and compare on/off frequencies
	float sumOn = (float)counterTreeRoot->getCounter(COUNTERIDX_ON);
	float sumOff = (float)counterTreeRoot->getCounter(COUNTERIDX_OFF);

	checkNode(counterTreeRoot,sumOn,sumOff,7,callback);
}


// ------------------------------------------- learning related functions

float TransitionStat::getNodePrecision(SequenceCounterTreeNode *node)
{
	int onRootNum = counterTreeRoot->getCounter(COUNTERIDX_ON);
	int offRootNum = counterTreeRoot->getCounter(COUNTERIDX_OFF);
	int onNum = node->getCounter(COUNTERIDX_ON);
	int offNum = node->getCounter(COUNTERIDX_OFF);

	// The overall counter numbers for the two counters may be
	//	very different due to imbalances of the training sets,
	//	so relative on/off rates are used.
	float onRate = (float)onNum / (float)onRootNum;
	float offRate = (float)offNum / (float)offRootNum;
	float precision = onRate / (onRate+offRate);

	return precision;
}

float TransitionStat::getCombinedPrecision(SequenceCounterTreeNode *nodeA, SequenceCounterTreeNode *nodeB)
{
	int onRootNum = counterTreeRoot->getCounter(COUNTERIDX_ON);
	int offRootNum = counterTreeRoot->getCounter(COUNTERIDX_OFF);

	int onNumA = nodeA->getCounter(COUNTERIDX_ON);
	int offNumA = nodeA->getCounter(COUNTERIDX_OFF);

	int onNumB = nodeB->getCounter(COUNTERIDX_ON);
	int offNumB = nodeB->getCounter(COUNTERIDX_OFF);

	// The overall counter numbers for the two counters may be
	//	very different due to imbalances of the training sets,
	//	so relative on/off rates are used.
	float onRate = (float)(onNumA+onNumB) / (float)(2*onRootNum);
	float offRate = (float)(offNumA+offNumB) / (float)(2*offRootNum);
	float precision = onRate / (onRate+offRate);

	return precision;
}

// returns whether nodeA and nodeB may be combined into one if minPrecision must be preserved
//	in every case where it was above the threshold originally.
bool TransitionStat::checkCanCombineNodes(
	SequenceCounterTreeNode *nodeA, SequenceCounterTreeNode *nodeB,
	float minPrecision, bool hasHighPrecisionParent)
{
	// Check for NULL input values
	if (nodeA == NULL && nodeB == NULL)
	{
		return true;	// No contradiction, as none of the nodes exists. (Possible parents may be combined.)
	}

	// If one of them is NULL, they can only be merged if the other is low-precision
	if (nodeA == NULL && nodeB != NULL)
	{
		float precisionB = getNodePrecision(nodeB);
		if (precisionB>=minPrecision)
		{
			return false;	// Cannot merge NULL into high-precision node
		}
		else
		{
			if (hasHighPrecisionParent)
				return false;	// Had a high-precision parent, so no merge with low-precision is allowed
			else
				return true;	// Had no high-precision parent, may merge low-precision and NULL...
		}
	}
	if (nodeA != NULL && nodeB == NULL)
	{
		float precisionA = getNodePrecision(nodeA);
		if (precisionA>=minPrecision)
			return false;	// Cannot merge NULL into high-precision node
		else
		{
			if (hasHighPrecisionParent)
				return false;	// Had a high-precision parent, so no merge with low-precision is allowed
			else
				return true;	// Had no high-precision parent, may merge low-precision and NULL...
		}
	}

	// Compare the nodes themselves
	bool nextHasHighPrecisionParent = false;
	float precisionA = getNodePrecision(nodeA);
	float precisionB = getNodePrecision(nodeB);
	if (precisionA>=minPrecision || precisionB>=minPrecision)
	{
		// Any of the original nodes is over the minimal precision,
		//	so the result has to be as well...
		float precisionAB = getCombinedPrecision(nodeA, nodeB);
		if (precisionAB<minPrecision)
		{
			return false;	// Node A and B cannot be combined due to minPrecision violation.
		}
		nextHasHighPrecisionParent = true;
	}

	// Go along every child and compare them
	for(unsigned int i=0; i<inputValueNumber; i++)
	{
		SequenceCounterTreeNode *childOfA = nodeA->getChildNode(i);
		SequenceCounterTreeNode *childOfB = nodeB->getChildNode(i);
		if (!checkCanCombineNodes(
			childOfA,
			childOfB,
			minPrecision, nextHasHighPrecisionParent))
		{
			return false;	// Children cannot be combined
		}
	}
	return true;
}

void TransitionStat::combineNodes(SequenceCounterTreeNode *nodeA, SequenceCounterTreeNode *nodeB)
{
	SequenceCounterTreeNode::combineNodes(counterTreeRoot,nodeA,nodeB);
}

void TransitionStat::optimizeGraph()
{


}
