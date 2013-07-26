#ifndef __PIXELPRECISIONCALCULATOR_H
#define __PIXELPRECISIONCALCULATOR_H
#include "SequenceCounterTreeNode.h"

#define STATUS_HIGHPRECISION	1
#define STATUS_LOWPRECISION		0

namespace smeyel
{
	class PixelPrecisionCalculator
	{
		unsigned int counterIdxOn;
		unsigned int counterIdxOff;

		/** Returns the precision for a given node.
			Used by checkCanCombineNodes().
		*/
		float getNodePrecision(SequenceCounterTreeNode *nodeA);
	public:
		PixelPrecisionCalculator(unsigned int counterIdxOn, unsigned int counterIdxOff)
		{
			this->counterIdxOn = counterIdxOn;
			this->counterIdxOff = counterIdxOff;
		}

		/** Sets node status based on precision */
		void setPrecisionStatus(SequenceCounterTreeNode *node, float minPrecision);
	};
}

#endif
