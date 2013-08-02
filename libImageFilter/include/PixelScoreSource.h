#ifndef __PIXELSCORESOURCE_H
#define __PIXELSCORESOURCE_H

namespace smeyel
{
	/** Abstract class representing the source from which the score of a pixel
		can be retrieved.
	*/
	class PixelScoreSource
	{
	public:
		/** Starts a new statistics sequence. Call for example at the beginning of image lines. */
		virtual void startNewSequence()=0;
		/** Returns the score for a given pixel value. Meant to be called sequentially for every pixel. */
		virtual unsigned char getScoreForValue(unsigned int value)=0;
	};
}

#endif
