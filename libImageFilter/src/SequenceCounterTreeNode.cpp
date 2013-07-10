
#include "SequenceCounterTreeNode.h"

using namespace std;
using namespace smeyel;

int SequenceCounterTreeNode::nextFreeNodeID = 0;

SequenceCounterTreeNode::SequenceCounterTreeNode(const int inputValueNumber)
{
	children = new SequenceCounterTreeNode*[inputValueNumber];
	for(int i=0; i<inputValueNumber; i++)
	{
		children[i]=NULL;
	}
	for(int i=0; i<MAXNODECOUNTERNUM; i++)
	{
		counter[i]=0;
	}
	this->inputValueNumber = inputValueNumber;
	auxScore = 0;

	nodeID = nextFreeNodeID;
	nextFreeNodeID++;
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
		children[inputValue] = new SequenceCounterTreeNode(inputValueNumber);
	}
	return children[inputValue];
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
	SequenceCounterTreeNode *child = getChildNode(*inputValues,createIfNotExisting);
	if (child)
	{
		return child->getNode(inputValues+1,numberOfValues-1,createIfNotExisting);
	}
	return NULL;
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
int SequenceCounterTreeNode::calculateSubtreeCounters(int counterIdx)
{
	OPENCV_ASSERT(counterIdx<MAXNODECOUNTERNUM,"SequenceCounterTreeNode.getAndStoreSubtreeSumCounter","Counter IDX > max!");
	OPENCV_ASSERT(counterIdx>=0,"SequenceCounterTreeNode.getAndStoreSubtreeSumCounter","Counter IDX negative!");

	int childrenSum=0;
	for(int i=0; i<inputValueNumber; i++)
	{
		// Cannot use getChildNode() as that would create non-existing nodes infinite long...
		if (children[i]!=NULL)
		{
			childrenSum += children[i]->calculateSubtreeCounters(counterIdx);
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

void SequenceCounterTreeNode::showCompactRecursive(int indent, int maxCounterIdx, vector<string> *inputValueNames)
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
	cout << "(NodeID="<<nodeID<<", status="<<status<<")" << endl;
	// show children
	for(int i=0; i<inputValueNumber; i++)
	{
		if (children[i])
		{
			writeIndent(indent);
			if (inputValueNames)
			{
				cout << (*inputValueNames)[i];
			}
			else
			{
				cout << i;
			}
			cout << ": ";

			children[i]->showCompactRecursive(indent+1,maxCounterIdx,inputValueNames);
		}
	}
}

void SequenceCounterTreeNode::replaceChildPointer(SequenceCounterTreeNode *oldNode, SequenceCounterTreeNode *newNode)
{
	for(int i=0; i<inputValueNumber; i++)
	{
		if (children[i] == oldNode)
		{
			children[i] = newNode;
		}
	}
}

void SequenceCounterTreeNode::redirectParents(SequenceCounterTreeNode *startNode,SequenceCounterTreeNode *oldNode,SequenceCounterTreeNode *newNode)
{
	if (startNode==NULL)
	{
		return;	// nothing to do...
	}
	// Redirect children (before modifying children pointers!) and this node
	for(int i=0; i<startNode->inputValueNumber; i++)
	{
		redirectParents(startNode->getChildNode(i),oldNode,newNode);
		// Now it is safe to change the child pointer (if necessary)
	}

	startNode->replaceChildPointer(oldNode, newNode);
}

SequenceCounterTreeNode *SequenceCounterTreeNode::getMergedNode(SequenceCounterTreeNode *root, SequenceCounterTreeNode *nodeA,SequenceCounterTreeNode *nodeB)
{
	assert(root!=NULL);
	if (nodeA == root || nodeB == root)
	{
		throw new std::exception("SequenceCounterTreeNode::getMergedNode: Root node cannot be combined!");
	}

	// If any of the nodes is NULL, simply return the other one
	if (nodeA == NULL)
		return nodeB;
	if (nodeB == NULL)
		return nodeA;

	if (nodeA == nodeB)	// Possible due to previous merges...
	{
		return nodeA;
	}

	// Create new, combined node
	SequenceCounterTreeNode *newNode = new SequenceCounterTreeNode(nodeA->inputValueNumber);

	// Combine children (without modifying parent which is already replaced...)
	for(int i=0; i<root->inputValueNumber; i++)
	{
		newNode->children[i] = getMergedNode(root,nodeA->children[i],nodeB->children[i]);
	}
	// Merge other fields
	newNode->auxScore = 0;	// Cannot be merged! (!)
	for(int i=0; i<MAXNODECOUNTERNUM; i++)
	{
		newNode->counter[i] = nodeA->counter[i] + nodeB->counter[i];
	}

	// Redirect parent nodes (if exist)
	redirectParents(root,nodeA,newNode);
	redirectParents(root,nodeB,newNode);

	return newNode;
}

void SequenceCounterTreeNode::combineNodes(SequenceCounterTreeNode *root, SequenceCounterTreeNode *nodeA, SequenceCounterTreeNode *nodeB)
{
	SequenceCounterTreeNode *mergedNode = SequenceCounterTreeNode::getMergedNode(root,nodeA,nodeB);
	redirectParents(root,nodeA,mergedNode);
	redirectParents(root,nodeB,mergedNode);
}

bool SequenceCounterTreeNode::isParentOf(SequenceCounterTreeNode *node)
{
	if (this==node)
	{
		return true;
	}
	for(int i=0; i<inputValueNumber; i++)
	{
		if (children[i]!=NULL)
		{
			if (children[i]->isParentOf(node))
			{
				return true;
			}
		}
	}
	return false;
}

bool SequenceCounterTreeNode::cut(unsigned int parentStatus)
{
	//cout << "--- Cut@" << this->nodeID << " parentStatus=" << parentStatus << " status=" << status << endl;
	// check this node
	bool isStatusEqual = (this->status == parentStatus);

	// Recursive check for all children
	bool allRemoved = true;
	for(int i=0; i<inputValueNumber; i++)
	{
		SequenceCounterTreeNode *child = children[i];
		if (child)
		{
			// Children are checked with this->status in every case.
			bool result = child->cut(this->status);

			if (!result)
			{
				//cout << "Cut@" << this->nodeID << " child " << i << ": keep" << endl;
				allRemoved = false;	// Could not remove all children
			}
			else
			{
				//cout << "Cut@" << this->nodeID << " child " << i << ": DEL" << endl;
				// Remove child
				delete child;
				children[i] = NULL;
			}
		}
	}

	// THis node can be removed if
	//	- all children are removed, and
	//	- the auxScore is the same as of the parent (caller)
	if (allRemoved && isStatusEqual)
	{
		//cout << "Cut@" << this->nodeID << " removeable" << endl;
		return true;
	}
	//cout << "Cut@" << this->nodeID << " not removeable" << endl;
	return false;
}
