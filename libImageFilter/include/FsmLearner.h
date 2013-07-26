#ifndef __FSMLEARNER_H
#define __FSMLEARNER_H

#include "TransitionStat.h"

#define STATUS_HIGHPRECISION	1
#define STATUS_LOWPRECISION		0

using namespace smeyel;

namespace smeyel
{
	/** Used to calculate a transition probability statistic from a sequence of values. */
	class FsmLearner : public TransitionStat
	{
		/** Returns the precision for a given node.
			Used by checkCanCombineNodes().
		*/
		float getNodePrecision(SequenceCounterTreeNode *nodeA);
		/** Returns the combined precision of two nodes (if they would be merged together.)
			Used by checkCanCombineNodes
		*/
		float getCombinedPrecision(SequenceCounterTreeNode *nodeA, SequenceCounterTreeNode *nodeB);

	public:
		/** Constructor
			@param inputValueNumber	The number of possible input values, also the maximal input value + 1.
			@param markovChainOrder	The length of history taken into account (order of Markov Chain)
			@param initialValue		Inputs prior to first added value are considered to be this value.
				Similar to assumed color of pixels outside the image boundaries.
			@param runLengthTransformConfigFile	Optional config file name for TransitionStat.RunLengthTransform.
				If omitted, this->runLengthTransform.load() has to be used.
		*/
		FsmLearner(const unsigned int inputValueNumber, const unsigned int markovChainOrder, const unsigned int initialValue, const char *runLengthTransformConfigFile = NULL);

	private:
		/** Check whether two nodes can be combined together while maintaining minPrecision.
			Subtrees are also checked.
		*/
		bool checkCanCombineNodes(SequenceCounterTreeNode *nodeA, SequenceCounterTreeNode *nodeB,
			float minPrecision, bool hasHighPrecisionParent=false);
		
		/** Wrapper to SequenceCounterTreeNode::combineNodes
			Used by optimizeGraph
		*/
		void combineNodes(SequenceCounterTreeNode *nodeA, SequenceCounterTreeNode *nodeB);

		/** Collects all nodes into given vector.
			Used to track the nodes not linked anymore after optimizations.
			The order of nodes starts from the leaves, then their common roots, than other leaves etc.
		*/
		void collectNodesBackwards(vector<SequenceCounterTreeNode *> *allNodes, SequenceCounterTreeNode *node);

		/** Checks all pairs of nodes for possible merging using checkCanCombineNodes(). */
		pair<SequenceCounterTreeNode *,SequenceCounterTreeNode *>
			checkAllCombinationsForMerge(vector<SequenceCounterTreeNode *> *allNodes, float minPrecision);

		/** Deletes the nodes present in oldNodes but not present in newNodes.
			Used to delete unused nodes. The required lists may be created using collectNodesBackwards().
		*/
		void deleteRemovedNodes(
			vector<SequenceCounterTreeNode *> *oldNodes,
			vector<SequenceCounterTreeNode *> *newNodes);

	public:
		/** Optimizing for precision, re-organizes the graph by merging nodes. */
		void mergeNodesForPrecision(vector<string> *inputValueNames);

		/** Sets node status based on precision */
		void setPrecisionStatus(SequenceCounterTreeNode *node, float minPrecision);
	};
}

#endif
