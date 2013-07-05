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
		/** Array of child node pointers. */
		SequenceCounterTreeNode **children;
		SequenceCounterTreeNode *parent;
		int inputValueNumber;
		int counter[MAXNODECOUNTERNUM];

		void writeIndent(int indent);
	public:

		SequenceCounterTreeNode(const int inputValueNumber, SequenceCounterTreeNode* parentNode);
		~SequenceCounterTreeNode();
		SequenceCounterTreeNode *getChildNode(const unsigned int inputValue);
		SequenceCounterTreeNode *getParentNode();

		/**
			Nodes are created on demand, return value is never NULL.
		*/
		SequenceCounterTreeNode *getNode(const unsigned int *inputValues, const int numberOfValues);

		void incrementCounter(int counterIdx);

		int getCounter(int counterIdx);

		int getSubtreeSumCounter(int counterIdx);

		/** Overwrites counter of current node with sum of children, except if there are no children (or sum is 0). */
		int getAndStoreSubtreeSumCounter(int counterIdx);

		void showRecursive(int indent, int maxCounterIdx, bool showNullChildren);

		void showCompactRecursive(int indent, int maxCounterIdx);
	};
}


#endif
