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

		/** Used by getLengthEncodedSequence() as output buffer.*/
		unsigned int *lengthEncodingOutputBuffer;

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

		/** Removes any states or input sequences stored in internal buffers. */
		void startNewSequence();

		// Used by runlengthEncodeSequence();
		unsigned int appendEncodedTimes(unsigned int *target, unsigned int value, unsigned int runlength);

		// May be used by addValue and getScoreForValue
		// Uses lengthEncodingOutputBuffer as output. Length of output is the return value.
		unsigned int runlengthEncodeSequence();

		/**	Adds a new value to the sequence.
			Call this function with every input sequence element to create the statistic.
			@param	inputValue	The value of the next element of the sequence the statistic is based on.
			@param	isTargetArea	Shows whether the current element is inside a target area or not.
		*/
		void addValue(const unsigned int inputValue, const bool isTargetArea);

		/** Using incrementally just like addValue(), it returns the
			auxiliary score value of the next node.
		*/
		unsigned char getScoreForValue(const unsigned int inputValue);

		// --- Higher level functions
	private:
		/** */
		unsigned char lastValue;
		/** Was the last received value inside the target area? */
		bool lastIsTargetArea;

		/** Number of times the last value is continuously repeated. */
		//int runLength;

		/** The last valid score. */
		unsigned char lastScore;
		/** Like addValue(), but adds a runlength quantizing feature. */
		void addRunLengthQuantizedValue(unsigned char value, bool isTargetArea);
		/** getScoreForValue() replacement for the quantized runlength case. */
		unsigned char getScoreForRunLengthQuantizedValue(unsigned char value);
		/** Checks wether the given node is a suitable classifier between target and background areas. */
		void checkNode(SequenceCounterTreeNode *node, float sumOn, float sumOff, int maxInputValue, notifycallbackPtr callback);

	public:
		static bool useRunLengthEncoding;

		/** Feeds a CV_81C1 (like LUT) image using addValue pixel-by-pixel. */
		void addImage(cv::Mat &image, bool isOn);
		void addImage(cv::Mat &image, cv::Rect &onRect);

		/** When training a classifier using checkNode(), the minimal required precision for suitablility. */
		float trainMinPrecision;
		/** Used by checkNode(), the minimal number of samples for suitablility. */
		int trainMinSampleNum;
		/** Uses checkNode() to find suitable classifier sequences to recognize the target area. */
		void findClassifierSequences(notifycallbackPtr callback);

		/** Retrieves the score with getScoreForValue() for every pixel. */
		void getScoreMaskForImage(cv::Mat &src, cv::Mat &dst);

		void verboseScoreForImageLocation(cv::Mat &src, cv::Point pointToCheck);

		// ----------- Debug helpers
		void showBufferContent(const char *bufferName, unsigned int *buffer, unsigned int length);
	};
}

#endif
