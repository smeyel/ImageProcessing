#ifndef __PIXELSCORESOURCE_H
#define __PIXELSCORESOURCE_H

namespace smeyel
{
	class PixelScoreSource
	{
	public:
		virtual void startNewSequence()=0;
		virtual unsigned char getScoreForValue(unsigned int value)=0;
	};

}

#endif
