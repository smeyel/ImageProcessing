#ifndef __PIXELPRECISIONCALCULATOR_H
#define __PIXELPRECISIONCALCULATOR_H
#include "SequenceCounterTreeNode.h"

#define STATUS_HIGHPRECISION	1
#define STATUS_LOWPRECISION		0

namespace smeyel
{
	/** Sets the node status and auxScore in a SequenceCounterTreeNode tree
		based on precision.

		Usage:
		- Instantiate and define counter indices for "ON" and "OFF" cases.
		- call setPrecisionStatus() on the root node of your counter tree.
	*/
	class PixelPrecisionCalculator
	{
		/** Index of counter in the SequenceCounterTreeNode for cases inside target areas. */
		unsigned int counterIdxOn;
		/** Index of counter in the SequenceCounterTreeNode for cases outside target areas. */
		unsigned int counterIdxOff;

		/** Returns the precision for a given node.
			Used by setPrecisionStatus().
		*/
		float getNodePrecision(SequenceCounterTreeNode *nodeA);
	public:
		PixelPrecisionCalculator(unsigned int counterIdxOn, unsigned int counterIdxOff)
		{
			this->counterIdxOn = counterIdxOn;
			this->counterIdxOff = counterIdxOff;
		}

		/** Sets node's status and auxScore based on precision.
			Recursively continues into the whole subtree.
			@param node	Root node.
			@param minPrecision	Minimal precision allowing positive status.
		*/
		void setPrecisionStatus(SequenceCounterTreeNode *node, float minPrecision);
	};
}

#endif
