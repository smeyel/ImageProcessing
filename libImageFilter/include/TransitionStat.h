#ifndef __TRANSITIONSTAT_H
#define __TRANSITIONSTAT_H
//#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)

#include "SequenceCounterTreeNode.h"
#include "RunLengthTransform.h"
#include "PixelScoreSource.h"

using namespace smeyel;

// Type of callback for promising nodes
// TODO: needed?
typedef void (*notifycallbackPtr)(SequenceCounterTreeNode *node, float precision);

#define COUNTERIDX_ON		0
#define COUNTERIDX_OFF		1

namespace smeyel
{
	/** Used to calculate the occurrence numbers of various value transition sequences. */
	// TODO: rename to SequenceStat, together with "ImagePixelSequenceStat" from ImageTransitionStat
	class TransitionStat : public PixelScoreSource
	{
	protected:
		/** Array containing the last input values. Length corresponds the order of the markov chain. */
		unsigned int *lastValues;
		/** Input values considered before the first input. */
		// TODO: not needed? getScoreForValue() returns 0 in such cases...
		//unsigned int initialValue;
		/** The number of possible input values, also the maximal input value + 1. */
		unsigned int inputValueNumber;
		/** The length of history taken into account (order of Markov Chain), size of lastValues array. */
		unsigned int markovChainOrder;

		/** Used to decide whether initial values are still in the lastValues array. */
		unsigned int valueNumberSinceSequenceStart;

	public: // for DEBUG
		/** Root node of counter tree */
		SequenceCounterTreeNode *counterTreeRoot;

	public:
		RunLengthTransform runLengthTransform;

		/**
			@param inputValueNumber	The number of possible input values, also the maximal input value + 1.
			@param markovChainOrder	The length of history taken into account (order of Markov Chain)
			@param initialValue		Inputs prior to first added value are considered to be this value.
				Similar to assumed color of pixels outside the image boundaries.
		*/
		TransitionStat(const unsigned int inputValueNumber, const unsigned int markovChainOrder, const char *runLengthTransformConfigFile = NULL);

		~TransitionStat();

		/** PixelScoreSource members
			@{
		*/
		/** Removes any states or input sequences stored in internal buffers. */
		virtual void startNewSequence();

		/** Using incrementally just like addValue(), it returns the
			auxiliary score value of the next node.
		*/
		virtual unsigned char getScoreForValue(unsigned int inputValue);
		/** }@ */

		/**	Adds a new value to the sequence.
			Call this function with every input sequence element to create the statistic.
			@param	inputValue	The value of the next element of the sequence the statistic is based on.
			@param	isTargetArea	Shows whether the current element is inside a target area or not.
		*/
		void addValue(const unsigned int inputValue, const bool isTargetArea);

		// --- Higher level functions
		void balanceCounter(unsigned int balancedCounterIdx, unsigned int withRespectToCounterIdx)
		{
			float onSum = (float)counterTreeRoot->calculateSubtreeCounters(balancedCounterIdx);
			float offSum = (float)counterTreeRoot->calculateSubtreeCounters(withRespectToCounterIdx);
			float multiplier = offSum / onSum;
			counterTreeRoot->multiplySubtreeCounters(balancedCounterIdx, multiplier);
		}

	public:
		static bool useRunLengthEncoding;

		// ----------- Debug helpers
		void showBufferContent(const char *bufferName, unsigned int *buffer, unsigned int length);
	};
}

#endif
