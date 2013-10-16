#include <cstdlib>
#include <fstream>
#include "RunLengthTransform.h"

using namespace smeyel;

unsigned int RunLengthTransform::appendEncodedTimes(unsigned int *target, unsigned int value, unsigned int runlength, int maxSpaceInOutputBuffer)
{
	int encodedRunLength = 0;
	for(int limitIdx=0; limitIdx<limitNumber; limitIdx++)
	{
		if (limits[limitIdx]<=runlength)
		{
			encodedRunLength = limitIdx + 1;	// 0th element is the minimal value for 1 output!
		}
		else
		{
			break;	// No need to go further
		}
	}
	if (encodedRunLength > maxSpaceInOutputBuffer)
	{
		encodedRunLength = maxSpaceInOutputBuffer;	// TODO: Should this return a warning or error?!
	}
	for (int i=0; i<encodedRunLength; i++)
	{
		*target = value;
		target++;
	}
	return encodedRunLength;
}

RunLengthTransform::RunLengthTransform()
{
	internalOutputBuffer = NULL;
	outputBufferSize = 0;
	inputBufferPtr = NULL;
	inputBufferSize = 0;
	limits = NULL;
	limitNumber = 0;
}

RunLengthTransform::~RunLengthTransform()
{
	if (internalOutputBuffer)
	{
		delete internalOutputBuffer;
	}
	if (limits)
	{
		delete limits;
	}
	internalOutputBuffer = NULL;
	outputBufferSize = 0;
	limits = NULL;
	limitNumber = 0;
}

void RunLengthTransform::createInternalOutputBuffer(int size)
{
	if (internalOutputBuffer != NULL)
	{
		delete internalOutputBuffer;
	}
	internalOutputBuffer = new unsigned int[size];
	outputBufferSize = size;
}

unsigned int *RunLengthTransform::getInternalOutputBuffer(int size)
{
	if (size>0)
	{
		createInternalOutputBuffer(size);
	}
	return internalOutputBuffer;
}

void RunLengthTransform::setInputBuffer(unsigned int *inputBuffer, int inputBufferSize)
{
	this->inputBufferPtr = inputBuffer;
	this->inputBufferSize = inputBufferSize;
}

unsigned int RunLengthTransform::transform()
{
	return transform(inputBufferPtr, inputBufferSize, internalOutputBuffer, outputBufferSize);
}

// Use by addValue and getScoreForValue
unsigned int RunLengthTransform::transform(unsigned int *inputBuffer, int inputLength, unsigned int *outputBuffer, int maxOutputLength)
{
	// Transforms the input into the output buffer.
	// The length of internal homogeneous value sequences are encoded into a few distinct values.

	unsigned int outputLength = 0;
	unsigned int prevValue = *inputBuffer;
	unsigned int runlength = 0;
	for(int i=0; i<inputLength; i++)
	{
		if (inputBuffer[i]==prevValue)
		{
			runlength++;
		}
		else
		{
			unsigned int encodedRunLength = appendEncodedTimes(outputBuffer,prevValue,runlength, maxOutputLength-outputLength);
			outputLength += encodedRunLength;
			outputBuffer += encodedRunLength;

			// Update lastValues
			prevValue = inputBuffer[i];
			runlength = 1;
		}
	}
	// Last sequence is not encoded yet, do it now.
	unsigned int encodedRunLength = appendEncodedTimes(outputBuffer,prevValue,runlength,maxOutputLength-outputLength);
	outputLength += encodedRunLength;

	return outputLength;
}

void RunLengthTransform::save(const char *filename)
{
	std::ofstream file;
	file.open(filename);
	file << limitNumber << " ";
	for(int i=0; i<limitNumber; i++)
	{
		file << this->limits[i] << " ";
	}
	file.close();
}

void RunLengthTransform::load(const char *filename)
{
	if (this->limits != NULL)
	{
		delete this->limits;
		this->limits = NULL;
	}
	std::ifstream file;
	file.open(filename);
//	int value;
	file >> limitNumber;
	this->limits = new unsigned int[limitNumber];

	for(int i=0; i<limitNumber; i++)
	{
		file >> this->limits[i];
	}
	file.close();
}

