#include "TransitionStat.h"

using namespace smeyel;

TransitionStat::TransitionStat(const unsigned int inputValueNumber, const unsigned int markovChainOrder, const unsigned int initialValue)
{
	counterTreeRoot = new SequenceCounterTreeNode(inputValueNumber,NULL);
	lastValues = new unsigned int[markovChainOrder];
	this->inputValueNumber = inputValueNumber;
	this->markovChainOrder = markovChainOrder;
	this->initialValue = initialValue;
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