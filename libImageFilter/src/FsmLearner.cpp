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
	float onNum = (float)node->getCounter(COUNTERIDX_ON);
	float offNum = (float)node->getCounter(COUNTERIDX_OFF);
	float precision = onNum / (onNum+offNum);
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


void FsmLearner::collectNodesBackwards(vector<SequenceCounterTreeNode *> *allNodes, SequenceCounterTreeNode *node)
{
	if (node==NULL)
	{
		return;
	}
	int n = node->getInputValueNumber();
	// first, recursive call
	for(int i=0; i<n; i++)
		collectNodesBackwards(allNodes, node->getChildNode(i));

	// second, add after all children are added
	for(unsigned int i=0; i<allNodes->size(); i++)
		if ((*allNodes)[i]==node)
			return;	// Already added
	allNodes->push_back(node);
}

pair<SequenceCounterTreeNode *,SequenceCounterTreeNode *>
	FsmLearner::checkAllCombinationsForMerge(vector<SequenceCounterTreeNode *> *allNodes, float minPrecision)
{
	int inputValueNumber = (*allNodes)[0]->getInputValueNumber();
	int nodeNumber = allNodes->size();

	for(unsigned int a=1; a<allNodes->size(); a++)	// "a" is always the next new node
	{
		for(unsigned int b=0; b<a; b++)	// "b" is iterating on all previously checked nodes
		{
			assert(a!=b);

			// Check nodes a and b
			SequenceCounterTreeNode *nodeA = (*allNodes)[a];
			SequenceCounterTreeNode *nodeB = (*allNodes)[b];

			int idA = nodeA->getNodeID();
			int idB = nodeA->getNodeID();

			if (nodeA->getNodeID() == nodeB->getNodeID())
			{
				continue;	// May happen due to previous merges...
			}

			// They cannot be parents of each other
			if (nodeA->isParentOf(nodeB) || nodeB->isParentOf(nodeA))
			{
				continue;
			}

			cout << "--- Checking node pair: " << idA << " and " << idB << endl;

			bool canCombine = checkCanCombineNodes(nodeA,nodeB,minPrecision);
			if (canCombine)
			{
				cout << "COMBINE: " << nodeA->getNodeID() << " and " << nodeB->getNodeID() << endl;
				// Do not search for further combination possibilities...
				pair<SequenceCounterTreeNode *,SequenceCounterTreeNode *> result;
				result.first = nodeA;
				result.second = nodeB;
				return result;
			}
		}
	}
	return pair<SequenceCounterTreeNode *,SequenceCounterTreeNode *>(
		(SequenceCounterTreeNode *)NULL,(SequenceCounterTreeNode *)NULL);	// No more possibilities found
}

void FsmLearner::deleteRemovedNodes(
	vector<SequenceCounterTreeNode *> *oldNodes,
	vector<SequenceCounterTreeNode *> *newNodes)
{
	for(unsigned int i=0; i<oldNodes->size(); i++)
	{
		bool found = false;
		for(unsigned int j=0; j<newNodes->size(); j++)
		{
			if ((*newNodes)[j]==(*oldNodes)[i])
				found=true;
		}
		if (!found)
		{
			// First, disconnect all children, otherwise, delete goes on!!!
			(*oldNodes)[i]->disconnectAllChildren();
			delete (*oldNodes)[i];
			(*oldNodes)[i]=NULL;	// just to make sure...
		}
	}
}

void FsmLearner::mergeNodesForPrecision(vector<string> *inputValueNames)
{
	vector<SequenceCounterTreeNode *> *allNodes = new vector<SequenceCounterTreeNode *>();
	vector<SequenceCounterTreeNode *> *newAllNodes = new vector<SequenceCounterTreeNode *>();

	bool running=true;
	while (running)
	{
		allNodes->clear();
		collectNodesBackwards(allNodes,counterTreeRoot);

		pair<SequenceCounterTreeNode *,SequenceCounterTreeNode *> toCombine =
			checkAllCombinationsForMerge(allNodes,0.7F);	// TODO: do not hardwire the precision!
		if (toCombine.first==NULL || toCombine.second==NULL)
		{
			running=false;
			break;
		}

		int idA = toCombine.first->getNodeID();
		int idB = toCombine.second->getNodeID();
		cout << "------------- combining nodes... -------------" << endl;
		cout << "COMBING: " << toCombine.first->getNodeID() << " and " << toCombine.second->getNodeID() << endl;
		combineNodes(toCombine.first,toCombine.second);

		// TODO: delete should be done inside combineNodes!!!
		newAllNodes->clear();
		collectNodesBackwards(newAllNodes,counterTreeRoot);
		deleteRemovedNodes(allNodes,newAllNodes);

		counterTreeRoot->showCompactRecursive(0,1,inputValueNames);
	}

	delete allNodes;
	allNodes=NULL;
}

void FsmLearner::setPrecisionStatus(SequenceCounterTreeNode *node, float minPrecision)
{
	if (node==NULL)
	{
		return;
	}

	float precision = FsmLearner::getNodePrecision(node);
	if (precision >= minPrecision)
	{
		node->status = STATUS_HIGHPRECISION;
	}
	else
	{
		node->status = STATUS_LOWPRECISION;
	}
	node->auxScore = (unsigned char)(128.0F + (precision-minPrecision)/(1.0F-minPrecision)*127.0F);

	int n = node->getInputValueNumber();
	// first, recursive call
	for(int i=0; i<n; i++)
	{
		setPrecisionStatus(node->getChildNode(i), minPrecision);
	}
}
