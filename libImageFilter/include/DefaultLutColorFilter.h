#ifndef __DEFAULTLUTCOLORFILTER_H
#define __DEFAULTLUTCOLORFILTER_H
#include "LutColorFilter.h"

#include "DefaultColorCodes.h"

using namespace smeyel;

class DefaultLutColorFilter : public LutColorFilter
{
public:
	DefaultLutColorFilter();
	void init();
};

#endif
