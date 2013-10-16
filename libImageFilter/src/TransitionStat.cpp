#include <sstream>

#include "Logger.h"
#include "TransitionStat.h"

using namespace smeyel;
using namespace cv;

bool TransitionStat::useRunLengthEncoding = true;

TransitionStat::TransitionStat(const unsigned int inputValueNumber, const unsigned int markovChainOrder, const char *runLengthTransformConfigFile)
{
	counterTreeRoot = new SequenceCounterTreeNode(inputValueNumber);
	lastValues = new unsigned int[markovChainOrder];
	this->inputValueNumber = inputValueNumber;
	this->markovChainOrder = markovChainOrder;

	this->runLengthTransform.createInternalOutputBuffer(markovChainOrder);
	this->runLengthTransform.setInputBuffer(this->lastValues,markovChainOrder);

	if (runLengthTransformConfigFile != NULL)
	{
		this->runLengthTransform.load(runLengthTransformConfigFile);
	}

	startNewSequence();
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
		lastValues[i]=0;	// Not really important because of valueNumberSinceSequenceStart. 
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
	// Shift and update history (lastValues)
	for(unsigned int i=0; i<markovChainOrder-1; i++)
		lastValues[i]=lastValues[i+1];
	lastValues[markovChainOrder-1] = inputValue;

	// Increment the counter only if initial values are already shifted out of the lastValues array.
	if (valueNumberSinceSequenceStart>=markovChainOrder)
	{
		SequenceCounterTreeNode *node = NULL;

		if (useRunLengthEncoding)
		{
			// Input and output buffers already set up in the constructor.
			unsigned int length = this->runLengthTransform.transform();

			counterTreeRoot->incrementCountersAlongPath(this->runLengthTransform.getInternalOutputBuffer(),length,true,isTargetArea ? COUNTERIDX_ON : COUNTERIDX_OFF);
		}
		else
		{
			counterTreeRoot->incrementCountersAlongPath(lastValues,markovChainOrder,true,isTargetArea ? COUNTERIDX_ON : COUNTERIDX_OFF);
		}

	}
}

unsigned char TransitionStat::getScoreForValue(const unsigned int inputValue)
{
	OPENCV_ASSERT(inputValue<inputValueNumber,"TransitionStat.getScoreForValue","value > max!");
	valueNumberSinceSequenceStart++;
	// Update history (lastValues)
	for(unsigned int i=0; i<markovChainOrder-1; i++)
		lastValues[i]=lastValues[i+1];
	lastValues[markovChainOrder-1] = inputValue;

	// Get the node only if initial values are already shifted out of the lastValues array.
	if (valueNumberSinceSequenceStart>=markovChainOrder)
	{
		SequenceCounterTreeNode *node = NULL;

		if (useRunLengthEncoding)
		{
			unsigned int length = this->runLengthTransform.transform();
			node = counterTreeRoot->getNode(this->runLengthTransform.getInternalOutputBuffer(),length,true);
		}
		else
		{
			node = counterTreeRoot->getNode(lastValues,markovChainOrder,true);
		}

		if (node)
		{
			return node->auxScore;
		}
	}

	return 0;
}

void TransitionStat::showBufferContent(const char *bufferName, unsigned int *buffer, unsigned int length)
{
	cout << "Buffer [" << bufferName << "]: ";
	for(unsigned int i=0; i<length; i++)
	{
		cout << *buffer;
		buffer++;
	}
	cout << endl;
}
