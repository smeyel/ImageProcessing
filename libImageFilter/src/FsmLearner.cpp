#include "FsmLearner.h"

using namespace smeyel;
using namespace cv;

FsmLearner::FsmLearner(const unsigned int inputValueNumber, const unsigned int markovChainOrder, const unsigned int initialValue)
	: TransitionStat(inputValueNumber, markovChainOrder, initialValue)
{
	trainMinPrecision=0.9F;
	trainMinSampleNum=50;
}

// ------------------------------------------- learning related functions

float FsmLearner::getNodePrecision(SequenceCounterTreeNode *node)
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

float FsmLearner::getCombinedPrecision(SequenceCounterTreeNode *nodeA, SequenceCounterTreeNode *nodeB)
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
bool FsmLearner::checkCanCombineNodes(
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

void FsmLearner::combineNodes(SequenceCounterTreeNode *nodeA, SequenceCounterTreeNode *nodeB)
{
	SequenceCounterTreeNode::combineNodes(counterTreeRoot,nodeA,nodeB);
}
