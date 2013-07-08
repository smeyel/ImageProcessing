#ifndef __TRANSITIONSTAT_H
#define __TRANSITIONSTAT_H
//#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)

#include "SequenceCounterTreeNode.h"

using namespace smeyel;

// Type of callback for promising nodes
typedef void (*notifycallbackPtr)(SequenceCounterTreeNode *node, float precision);

#define COUNTERIDX_ON		0
#define COUNTERIDX_OFF		1

namespace smeyel
{
	/** Used to calculate a transition probability statistic from a sequence of values. */
	class TransitionStat
	{
	protected:
		/** Array containing the last input values. Length corresponds the order of the markov chain. */
		unsigned int *lastValues;
		/** Input values considered before the first input. */
		unsigned int initialValue;
		/** The number of possible input values, also the maximal input value + 1. */
		unsigned int inputValueNumber;
		/** The length of history taken into account (order of Markov Chain) */
		unsigned int markovChainOrder;

		/** Used to decide whether initial values are still in the lastValues array. */
		unsigned int valueNumberSinceSequenceStart;

	public: // for DEBUG
		/** Root node of counter tree */
		SequenceCounterTreeNode *counterTreeRoot;

	public:
		/**
			@param inputValueNumber	The number of possible input values, also the maximal input value + 1.
			@param markovChainOrder	The length of history taken into account (order of Markov Chain)
			@param initialValue		Inputs prior to first added value are considered to be this value.
				Similar to assumed color of pixels outside the image boundaries.
		*/
		TransitionStat(const unsigned int inputValueNumber, const unsigned int markovChainOrder, const unsigned int initialValue);

		~TransitionStat();

		void startNewSequence();

		/**	Adds a new value to the sequence.
			Call this function with every input sequence element to create the statistic.
			@param	inputValue	The value of the next element of the sequence the statistic is based on.
			@param	isTargetArea	Shows whether the current element is inside a target area or not.
		*/
		void addValue(const unsigned int inputValue, const bool isTargetArea);

		unsigned char getScoreForValue(const unsigned int inputValue);

		// Higher level functions
	private:
		unsigned char lastValue;
		bool lastIsTargetArea;
		int runLength;
		unsigned char lastScore;
		void addRunLengthQuantizedValue(unsigned char value, bool isTargetArea);
		unsigned char getScoreForRunLengthQuantizedValue(unsigned char value);
		void checkNode(SequenceCounterTreeNode *node, float sumOn, float sumOff, int maxInputValue, notifycallbackPtr callback);

	public:
		void addImage(cv::Mat &image, bool isOn);
		void getScoreMaskForImage(cv::Mat &src, cv::Mat &dst);

		float trainMinPrecision;
		int trainMinSampleNum;
		void findClassifierSequences(notifycallbackPtr callback);
	};
}


#endif
