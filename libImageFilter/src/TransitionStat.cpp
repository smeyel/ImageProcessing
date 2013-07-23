#include <sstream>

#include "Logger.h"
#include "TransitionStat.h"

using namespace smeyel;
using namespace cv;

bool TransitionStat::useRunLengthEncoding = true;

TransitionStat::TransitionStat(const unsigned int inputValueNumber, const unsigned int markovChainOrder, const unsigned int initialValue)
{
	counterTreeRoot = new SequenceCounterTreeNode(inputValueNumber);
	lastValues = new unsigned int[markovChainOrder];
	lengthEncodingOutputBuffer = new unsigned int[markovChainOrder];
	this->inputValueNumber = inputValueNumber;
	this->markovChainOrder = markovChainOrder;
	this->initialValue = initialValue;

	startNewSequence();

	lastValue = -1;
	lastIsTargetArea = false;
	//runLength = 0;
	lastScore = 0;

	trainMinPrecision=0.9F;
	trainMinSampleNum=50;

}

TransitionStat::~TransitionStat()
{
	delete counterTreeRoot;
	delete lastValues;
	delete lengthEncodingOutputBuffer;
	counterTreeRoot = NULL;
	lastValues = NULL;
	lengthEncodingOutputBuffer = NULL;
}

void TransitionStat::startNewSequence()
{
	for(unsigned int i=0; i<markovChainOrder; i++)
		lastValues[i]=initialValue;
	valueNumberSinceSequenceStart=0;
}

unsigned int TransitionStat::appendEncodedTimes(unsigned int *target, unsigned int value, unsigned int runlength)
{
	// Write to output a changed number of lastValue-s.
	int encodedRunLength = 0;
	if (runlength>=5)	encodedRunLength = 1;
	if (runlength>=10)	encodedRunLength = 2;
	if (runlength>=20)	encodedRunLength = 3;
	if (runlength>=50)	encodedRunLength = 4;
	for (int i=0; i<encodedRunLength; i++)
	{
		*target = value;
		target++;
	}
	return encodedRunLength;
}

// Use by addValue and getScoreForValue
unsigned int TransitionStat::runlengthEncodeSequence()
{
	// Transforms the input into the output buffer.
	// The length of internal homogeneous value sequences are encoded into a few distinct values.
	unsigned int *inputBuffer = this->lastValues;
	unsigned int *outputBuffer = this->lengthEncodingOutputBuffer;

	unsigned int targetLength = 0;
	unsigned int prevValue = *inputBuffer;
	unsigned int runlength = 0;
	for(int i=0; i<this->markovChainOrder; i++)
	{
		if (inputBuffer[i]==prevValue)
		{
			runlength++;
		}
		else
		{
			unsigned int encodedRunLength = appendEncodedTimes(outputBuffer,prevValue,runlength);
			targetLength += encodedRunLength;
			outputBuffer += encodedRunLength;

			// Update lastValues
			prevValue = inputBuffer[i];
			runlength = 1;
		}
	}

	unsigned int encodedRunLength = appendEncodedTimes(outputBuffer,prevValue,runlength);
	targetLength += encodedRunLength;

	return targetLength;
}

/**	Adds a new value to the sequence.
	Call this function with every input sequence element to create the statistic.
	@param	inputValue	The value of the next element of the sequence the statistic is based on.
	@param	isTargetArea	Shows whether the current element is inside a target area or not.
*/
void TransitionStat::addValue(const unsigned int inputValue, const bool isTargetArea)
{
	OPENCV_ASSERT(inputValue<inputValueNumber,"TransitionStat.addValue","value > max!");
	valueNumberSinceSequenceStart++;
	// Shift and update history (lastValues)
	for(unsigned int i=0; i<markovChainOrder-1; i++)
		lastValues[i]=lastValues[i+1];
	lastValues[markovChainOrder-1] = inputValue;

	// Increment the counter only if initial values are already shifted out of the lastValues array.
	if (valueNumberSinceSequenceStart>=markovChainOrder)
	{
		SequenceCounterTreeNode *node = NULL;

		if (useRunLengthEncoding)
		{
			// Debug
			//showBufferContent("LastValues",lastValues,markovChainOrder);

			unsigned int length = runlengthEncodeSequence();
			// Debug
			//showBufferContent("LenEncoded",lengthEncodingOutputBuffer,length);
			node = counterTreeRoot->getNode(lengthEncodingOutputBuffer,length,true);

			// DEBUG
/*			if (isTargetArea)
			{
				int oldOnCnt = node->getCounter(COUNTERIDX_ON);
				int oldOffCnt = node->getCounter(COUNTERIDX_OFF);
				stringstream ss;
				for(int i=0; i<length; i++)
				{
					ss << (int)(lengthEncodingOutputBuffer[i]);
				}
				string str = ss.str();
				const char *strPtr = str.c_str(); 
				LogConfigTime::Logger::getInstance()->Log(LogConfigTime::Logger::LOGLEVEL_VERBOSE,"addValue","INC %s for seq: %s, old cnt(on/off):%d/%d\n",(isTargetArea?"ON":"OFF"),strPtr,oldOnCnt,oldOffCnt);
			}*/
			// END OF DEBUG
		}
		else
		{
			node = counterTreeRoot->getNode(lastValues,markovChainOrder,true);
		}

		// Increment respective counter
		node->incrementCounter(isTargetArea ? COUNTERIDX_ON : COUNTERIDX_OFF);
	}
}

unsigned char TransitionStat::getScoreForValue(const unsigned int inputValue)
{
	OPENCV_ASSERT(inputValue<inputValueNumber,"TransitionStat.getScoreForValue","value > max!");
	valueNumberSinceSequenceStart++;
	// Update history (lastValues)
	for(unsigned int i=0; i<markovChainOrder-1; i++)
		lastValues[i]=lastValues[i+1];
	lastValues[markovChainOrder-1] = inputValue;

	// Get the node only if initial values are already shifted out of the lastValues array.
	if (valueNumberSinceSequenceStart>=markovChainOrder)
	{
		SequenceCounterTreeNode *node = NULL;

		if (useRunLengthEncoding)
		{
			unsigned int length = runlengthEncodeSequence();
			node = counterTreeRoot->getNode(lengthEncodingOutputBuffer,length,true);
		}
		else
		{
			node = counterTreeRoot->getNode(lastValues,markovChainOrder,true);
		}

		if (node)
		{
			return node->auxScore;
		}
	}

	return 0;
}

// Deprecated, use addImage(image,onRect) instead!
void TransitionStat::addImage(Mat &image, bool isOn)
{
	// Assert for only 8UC1 output images
	OPENCV_ASSERT(image.type() == CV_8UC1,"feedImageIntoTransitionStat","Image type is not CV_8UC1");

	// Go along every pixel and do the following:
	for (int row=0; row<image.rows; row++)
	{
		// Calculate pointer to the beginning of the current row
		const uchar *ptr = (const uchar *)(image.data + row*image.step);
		
		// Sequences are restarted at the beginning of every image row.
		startNewSequence();
		// Go along every BGR colorspace pixel
		for (int col=0; col<image.cols; col++)
		{
			unsigned char value = *ptr++;

			addValue(value,isOn);
		}	// end for col
	}	// end for row
}

void TransitionStat::addImage(Mat &image, Rect &onRect)
{
	// Assert for only 8UC1 output images
	OPENCV_ASSERT(image.type() == CV_8UC1,"feedImageIntoTransitionStat","Image type is not CV_8UC1");

	// Go along every pixel and do the following:
	for (int row=0; row<image.rows; row++)
	{
		// Calculate pointer to the beginning of the current row
		const uchar *ptr = (const uchar *)(image.data + row*image.step);
		
		// Sequences are restarted at the beginning of every image row.
		startNewSequence();
		// Go along every BGR colorspace pixel
		for (int col=0; col<image.cols; col++)
		{
			unsigned char value = *ptr++;

			bool isOn = onRect.contains(Point(col,row));
			addValue(value,isOn);
		}	// end for col
	}	// end for row
}

void TransitionStat::getScoreMaskForImage(Mat &src, Mat &dst)
{
	// Assert for only 8UC1 output images
	OPENCV_ASSERT(src.type() == CV_8UC1,"getScoreMaskForImage","src type is not CV_8UC1");
	OPENCV_ASSERT(dst.type() == CV_8UC1,"getScoreMaskForImage","dst type is not CV_8UC1");
	OPENCV_ASSERT(src.rows=dst.rows,"getScoreMaskForImage","src and dst size does not match");
	OPENCV_ASSERT(src.cols=dst.cols,"getScoreMaskForImage","src and dst size does not match");

	// Go along every pixel and do the following:
	for (int row=0; row<src.rows; row++)
	{
		// Calculate pointer to the beginning of the current row
		const uchar *srcPtr = (const uchar *)(src.data + row*src.step);
		uchar *dstPtr = (uchar *)(dst.data + row*dst.step);

		// Sequences are restarted at the beginning of every image row.
		startNewSequence();
		// Go along every BGR colorspace pixel
		for (int col=0; col<src.cols; col++)
		{
			unsigned char value = *srcPtr++;

			*dstPtr = getScoreForValue(value);
			//*dstPtr = getScoreForRunLengthQuantizedValue(value);
			dstPtr++;
		}	// end for col
	}	// end for row
}

// For debug purposes
void TransitionStat::verboseScoreForImageLocation(Mat &src, Point pointToCheck)
{
	// Assert for only 8UC1 output images
	OPENCV_ASSERT(src.type() == CV_8UC1,"getVerboseScoreForImageLocation","src type is not CV_8UC1");

	// Calculate pointer to the beginning of the current row
	uchar *srcPtr = (uchar *)(src.data + pointToCheck.y*src.step);

	if (pointToCheck.x < markovChainOrder)
	{
		cout << "Point to check may be too close the the image border (x<markovChainOrder)" << endl;
	}

	uchar *ptr = NULL;
	for(unsigned int i=0; i<markovChainOrder; i++)
	{
		ptr = srcPtr+pointToCheck.x-markovChainOrder+i;
		lastValues[i]=*ptr;
		*ptr = 0x00;	// Debug purposes
	}

	SequenceCounterTreeNode *node = NULL;

	cout << "Source and encoded buffer values:" << endl;
	showBufferContent("LastValues",lastValues,markovChainOrder);
	unsigned int length = runlengthEncodeSequence();
	showBufferContent("LenEncoded",lengthEncodingOutputBuffer,length);

	node = counterTreeRoot->getNode(lengthEncodingOutputBuffer,length,true);
	if (node)
	{
		cout << "Node status=" << node->status << ", auxScore=" << (int)(node->auxScore);
		cout << ", counters(on/off): " << node->getCounter(COUNTERIDX_ON) << "/" << node->getCounter(COUNTERIDX_OFF) << endl;
	}
	else
	{
		cout << "No available node..." << endl;
	}
}


// Used by findClassifierSequences() recursively.
void TransitionStat::checkNode(SequenceCounterTreeNode *node, float sumOn, float sumOff, int maxInputValue, notifycallbackPtr callback)
{
	int onNum = node->getCounter(COUNTERIDX_ON);
	int offNum = node->getCounter(COUNTERIDX_OFF);
	int sumNum = onNum+offNum;
	float onRate = (float)onNum / (float)sumOn;
	float offRate = (float)offNum / (float)sumOff;
	float precision = onRate / (onRate+offRate);
	if (precision >= trainMinPrecision && sumNum>=trainMinSampleNum )
	{
		(*callback)(node, precision);
	}
	// Continue on children
	for (int i=0; i<=maxInputValue; i++)
	{
		SequenceCounterTreeNode *child = node->getChildNode(i,false);
		if (child)
		{
			checkNode(child,sumOn,sumOff,maxInputValue,callback);
		}
	}
}

void TransitionStat::findClassifierSequences(notifycallbackPtr callback)
{
	// Recursively go along every sequence and compare on/off frequencies
	float sumOn = (float)counterTreeRoot->getCounter(COUNTERIDX_ON);
	float sumOff = (float)counterTreeRoot->getCounter(COUNTERIDX_OFF);

	checkNode(counterTreeRoot,sumOn,sumOff,7,callback);
}

void TransitionStat::showBufferContent(const char *bufferName, unsigned int *buffer, unsigned int length)
{
	cout << "Buffer [" << bufferName << "]: ";
	for(int i=0; i<length; i++)
	{
		cout << *buffer;
		buffer++;
	}
	cout << endl;
}
