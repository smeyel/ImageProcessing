#ifndef __FSMLEARNER_H
#define __FSMLEARNER_H

#include "TransitionStat.h"

using namespace smeyel;

namespace smeyel
{
	/** Used to calculate a transition probability statistic from a sequence of values. */
	class FsmLearner : public TransitionStat
	{
		// used by checkCanCombineNodes
		float getNodePrecision(SequenceCounterTreeNode *nodeA);
		// used by checkCanCombineNodes
		float getCombinedPrecision(SequenceCounterTreeNode *nodeA, SequenceCounterTreeNode *nodeB);

	public:
		/**
			@param inputValueNumber	The number of possible input values, also the maximal input value + 1.
			@param markovChainOrder	The length of history taken into account (order of Markov Chain)
			@param initialValue		Inputs prior to first added value are considered to be this value.
				Similar to assumed color of pixels outside the image boundaries.
		*/
		FsmLearner(const unsigned int inputValueNumber, const unsigned int markovChainOrder, const unsigned int initialValue);

	public:
		float trainMinPrecision;
		int trainMinSampleNum;
		void findClassifierSequences(notifycallbackPtr callback);

		// --------------- Graph tidy-up functions
		// returns change of number of recognized sequences if the two nodes are merged. minPrecision must be preserved
		//	in every case where it was above the threshold originally.

		// used by optimizeGraph
		bool checkCanCombineNodes(SequenceCounterTreeNode *nodeA, SequenceCounterTreeNode *nodeB,
			float minPrecision, bool hasHighPrecisionParent=false);
		// wrapper to SequenceCounterTreeNode::combineNodes
		// used by optimizeGraph
		void combineNodes(SequenceCounterTreeNode *nodeA, SequenceCounterTreeNode *nodeB);
	};
}



#endif