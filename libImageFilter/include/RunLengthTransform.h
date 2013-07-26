#ifndef __RUNLENGTHTRANSFORM_H
#define __RUNLENGTHTRANSFORM_H

namespace smeyel
{
	class RunLengthTransform
	{
		unsigned int *internalOutputBuffer;
		int outputBufferSize;

		unsigned int *inputBufferPtr;
		int inputBufferSize;

		unsigned int *limits;
		int limitNumber;

		unsigned int appendEncodedTimes(unsigned int *target, unsigned int value, unsigned int runlength, int maxlength);
	public:
		RunLengthTransform();

		~RunLengthTransform();

		void createInternalOutputBuffer(int size);

		unsigned int *getInternalOutputBuffer(int size = -1);

		void setInputBuffer(unsigned int *inputBuffer, int inputBufferSize);

		unsigned int runlengthEncodeSequence();

		unsigned int runlengthEncodeSequence(unsigned int *inputBuffer, int inputLength, unsigned int *outputBuffer, int maxOutputLength);

		/** Saves the transformation into a file.
			@param filename	The name of the file.
		*/
		void save(const char *filename);

		/** Loads the transformation from a file.
			@param filename	The name of the file.
		*/
		void load(const char *filename);
	};
}

#endif
