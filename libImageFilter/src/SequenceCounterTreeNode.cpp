
#include "SequenceCounterTreeNode.h"

using namespace std;
using namespace smeyel;

SequenceCounterTreeNode::SequenceCounterTreeNode(const int inputValueNumber, SequenceCounterTreeNode* parentNode)
{
	children = new SequenceCounterTreeNode*[inputValueNumber];
	for(int i=0; i<inputValueNumber; i++)
	{
		children[i]=NULL;
	}
	parent=parentNode;
	for(int i=0; i<MAXNODECOUNTERNUM; i++)
	{
		counter[i]=0;
	}
	this->inputValueNumber = inputValueNumber;
}

SequenceCounterTreeNode::~SequenceCounterTreeNode()
{
	for(int i=0; i<inputValueNumber; i++)
	{
		if (children[i]!=NULL)
		{
			delete children[i];
		}
	}
}

SequenceCounterTreeNode *SequenceCounterTreeNode::getChildNode(const unsigned int inputValue, bool createIfNotPresent)
{
	OPENCV_ASSERT(inputValue<(unsigned int)inputValueNumber,"SequenceCounterTreeNode.getChildNode","Input value is too high!");
	if (children[inputValue] == NULL && createIfNotPresent)
	{
		children[inputValue] = new SequenceCounterTreeNode(inputValueNumber,this);
	}
	return children[inputValue];
}

SequenceCounterTreeNode *SequenceCounterTreeNode::getParentNode()
{
	return this->parent;
}

int SequenceCounterTreeNode::getInputValueForChild(SequenceCounterTreeNode *child)
{
	for(int i=0; i<inputValueNumber; i++)
	{
		if (children[i]==child)
		{
			return i;
		}
	}
	return -1;
}

/**
	Nodes are created on demand if createIfNotExisting.
*/
SequenceCounterTreeNode *SequenceCounterTreeNode::getNode(const unsigned int *inputValues, const int numberOfValues, bool createIfNotExisting)
{
	if (numberOfValues == 0)
	{
		return this;
	}
	if (numberOfValues == 1)
	{
		// Return direct child node
		return getChildNode(*inputValues,createIfNotExisting);
	}
	// Recursive call
	return getChildNode(*inputValues,createIfNotExisting)->getNode(inputValues+1,numberOfValues-1,createIfNotExisting);
}

void SequenceCounterTreeNode::incrementCounter(int counterIdx)
{
	OPENCV_ASSERT(counterIdx<MAXNODECOUNTERNUM,"SequenceCounterTreeNode.incrementCounter","Counter IDX > max!");
	OPENCV_ASSERT(counterIdx>=0,"SequenceCounterTreeNode.incrementCounter","Counter IDX negative!");
	counter[counterIdx]++;
}

int SequenceCounterTreeNode::getCounter(int counterIdx)
{
	OPENCV_ASSERT(counterIdx<MAXNODECOUNTERNUM,"SequenceCounterTreeNode.getCounter","Counter IDX > max!");
	OPENCV_ASSERT(counterIdx>=0,"SequenceCounterTreeNode.getCounter","Counter IDX negative!");
	return counter[counterIdx];
}

int SequenceCounterTreeNode::getSubtreeSumCounter(int counterIdx)
{
	OPENCV_ASSERT(counterIdx<MAXNODECOUNTERNUM,"SequenceCounterTreeNode.getSubtreeSumCounter","Counter IDX > max!");
	OPENCV_ASSERT(counterIdx>=0,"SequenceCounterTreeNode.getSubtreeSumCounter","Counter IDX negative!");
	int sum = counter[counterIdx];
	for(int i=0; i<inputValueNumber; i++)
	{
		// Cannot use getChildNode() as that would create non-existing nodes infinite long...
		if (children[i]!=NULL)
		{
			sum += children[i]->getSubtreeSumCounter(counterIdx);
		}
	}
	return sum;
}

/** Overwrites counter of current node with sum of children, except if there are no children (or sum is 0). */
int SequenceCounterTreeNode::getAndStoreSubtreeSumCounter(int counterIdx)
{
	OPENCV_ASSERT(counterIdx<MAXNODECOUNTERNUM,"SequenceCounterTreeNode.getAndStoreSubtreeSumCounter","Counter IDX > max!");
	OPENCV_ASSERT(counterIdx>=0,"SequenceCounterTreeNode.getAndStoreSubtreeSumCounter","Counter IDX negative!");

	int childrenSum=0;
	for(int i=0; i<inputValueNumber; i++)
	{
		// Cannot use getChildNode() as that would create non-existing nodes infinite long...
		if (children[i]!=NULL)
		{
			childrenSum += children[i]->getAndStoreSubtreeSumCounter(counterIdx);
		}
	}

	if (childrenSum>0)
	{
		counter[counterIdx] = childrenSum;
	}

	return counter[counterIdx];
}

void SequenceCounterTreeNode::writeIndent(int indent)
{
	for(int i=0; i<indent; i++)
		cout << " ";
}

void SequenceCounterTreeNode::showRecursive(int indent, int maxCounterIdx, bool showNullChildren)
{
	OPENCV_ASSERT(maxCounterIdx<MAXNODECOUNTERNUM,"SequenceCounterTreeNode.showRecursive","Max counter IDX > max!");
	OPENCV_ASSERT(maxCounterIdx>=0,"SequenceCounterTreeNode.showRecursive","Max counter IDX negative!");
	writeIndent(indent);
	cout << "SequenceCounterTreeNode local counters:";
	for(int i=0; i<=maxCounterIdx; i++)
	{
		cout << " #" << i << "=" << counter[i];
	}
	cout << endl;
	// show children
	for(int i=0; i<inputValueNumber; i++)
	{
		if (children[i])
		{
			writeIndent(indent);
			cout << "Child " << i << ":" << endl;
			children[i]->showRecursive(indent+1,maxCounterIdx,showNullChildren);
		}
		else
		{
			if (showNullChildren)
			{
				writeIndent(indent);
				cout << "Child " << i << ": NULL" << endl;
			}
		}
	}
}

void SequenceCounterTreeNode::showCompactRecursive(int indent, int maxCounterIdx)
{
	OPENCV_ASSERT(maxCounterIdx<MAXNODECOUNTERNUM,"SequenceCounterTreeNode.showCompactRecursive","Max counter IDX > max!");
	OPENCV_ASSERT(maxCounterIdx>=0,"SequenceCounterTreeNode.showCompactRecursive","Max counter IDX negative!");
	for(int i=0; i<=maxCounterIdx; i++)
	{
		cout << counter[i];
		if (i<maxCounterIdx)
		{
			cout << "/";
		}
	}
	cout << endl;
	// show children
	for(int i=0; i<inputValueNumber; i++)
	{
		if (children[i])
		{
			writeIndent(indent);
			cout << i << ": ";
			children[i]->showCompactRecursive(indent+1,maxCounterIdx);
		}
	}
}
