#ifndef __SEQUENCECOUNTERTREENODE_H
#define __SEQUENCECOUNTERTREENODE_H

#include <opencv2/opencv.hpp>			// For OPENCV_ASSERT
//#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)

#include "LutColorFilter.h"

using namespace std;

#define MAXNODECOUNTERNUM	5

namespace smeyel
{
	class SequenceCounterTreeNode
	{
		static int nextFreeNodeID;

		int nodeID;
		/** Array of child node pointers. */
		SequenceCounterTreeNode **children;

		int inputValueNumber;
		int counter[MAXNODECOUNTERNUM];


		void writeIndent(int indent);
	public:
		unsigned char auxScore;	// May be used freely...

		SequenceCounterTreeNode(const int inputValueNumber);
		~SequenceCounterTreeNode();
		SequenceCounterTreeNode *getChildNode(const unsigned int inputValue, bool createIfNotPresent=false);

		int getInputValueForChild(SequenceCounterTreeNode *child);

		/**
			Nodes are created on demand, return value is never NULL.
		*/
		SequenceCounterTreeNode *getNode(const unsigned int *inputValues, const int numberOfValues, bool createIfNotExisting);

		void incrementCounter(int counterIdx);

		int getCounter(int counterIdx);

		int getSubtreeSumCounter(int counterIdx);

		/** Overwrites counter of current node with sum of children, except if there are no children (or sum is 0). */
		int calculateSubtreeCounters(int counterIdx);

		void showRecursive(int indent, int maxCounterIdx, bool showNullChildren);

		void showCompactRecursive(int indent, int maxCounterIdx, vector<string> *inputValueNames=NULL);

		int getNodeID()
		{
			return nodeID;
		}

		int getInputValueNumber()
		{
			return inputValueNumber;
		}

		void disconnectAllChildren()
		{
			for(int i=0; i<inputValueNumber; i++)
			{
				children[i]=NULL;
			}
		}

		bool isParentOf(SequenceCounterTreeNode *node);

		// --------- graph modification functions
	private:
		// used by redirectParents
		void replaceChildPointer(SequenceCounterTreeNode *oldNode, SequenceCounterTreeNode *newNode);
		// used by combineNodes
		static void redirectParents(SequenceCounterTreeNode *startNode,SequenceCounterTreeNode *oldNode,SequenceCounterTreeNode *newNode);
		// used by combineNodes
		static SequenceCounterTreeNode *getMergedNode(SequenceCounterTreeNode *root, SequenceCounterTreeNode *nodeA,SequenceCounterTreeNode *nodeB);
	public:
		static void combineNodes(SequenceCounterTreeNode *root, SequenceCounterTreeNode *nodeA, SequenceCounterTreeNode *nodeB);

	};
}

#endif
