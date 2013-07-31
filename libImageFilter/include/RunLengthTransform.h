#ifndef __RUNLENGTHTRANSFORM_H
#define __RUNLENGTHTRANSFORM_H

namespace smeyel
{
	/** Performs run length transformation: for a homogeneous sequence of values,
		the output contains the same values but the length of the sequence is modified.
		For example "122345555555" may be changed to "123455" where two values indicate
		"more than 4 values in original sequence".
	*/
	class RunLengthTransform
	{
		/** Internal output buffer for convenience, if you do not want to output the results
			into an external buffer. Initialized by createInternalOutputBuffer() and used by
			transform() when no parameters are assigned.
		*/
		unsigned int *internalOutputBuffer;
		/** Maximal size of the internal output buffer. */
		int outputBufferSize;

		/** Pointer to the (external) input buffer. Has to be assigned with
			setInputBuffer() prior to using transform() without parameters.
		*/
		unsigned int *inputBufferPtr;
		/** Size of the input buffer. Exactly this number of input values are processed. */
		int inputBufferSize;

		/** The list of runlength values where a next output element is written.
			For example the values "1 3 10" mean that in the output, 1 value means 1 in the original,
				1 values mean at least 3 in the original, and 3 values after each other mean at least
				10 values in the original input.
			May be written to and read from file using save() and load().
		*/
		unsigned int *limits;
		/** Number of limits. */
		int limitNumber;

		/** Appends an encoded number of values to the output buffer if there is still space (maxlength).
			@param target	The location in the output buffer to write to.
			@param value	The value to write.
			@param runlength	The current length of the run in the input buffer.
			@param maxlength	The number of values still available in the output buffer.
			@return	The number of values written.
			
			@warning If there is insufficient space in the output buffer, only the allowed number (maxlength)
				number of values are written. No warning is given about this case.
		*/
		unsigned int appendEncodedTimes(unsigned int *target, unsigned int value, unsigned int runlength, int maxlength);
	public:
		RunLengthTransform();

		~RunLengthTransform();

		/** Allocates an internal output buffer. */
		void createInternalOutputBuffer(int size);

		/** Returns the pointer to the internally allocated output buffer.
			If size is given, the buffer is resized.
			@warning Resizing the buffer may erase its content!
		*/
		unsigned int *getInternalOutputBuffer(int size = -1);

		/** Sets the input buffer to use. Only needed by transform() without parameters. */
		void setInputBuffer(unsigned int *inputBuffer, int inputBufferSize);

		/** Performs the transformation and returns the length of the output. */
		unsigned int transform();

		/** Performs the transformation and returns the length of the output. */
		unsigned int transform(unsigned int *inputBuffer, int inputLength, unsigned int *outputBuffer, int maxOutputLength);

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
