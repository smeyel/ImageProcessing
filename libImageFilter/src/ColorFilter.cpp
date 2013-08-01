#include "ColorFilter.h"
#include "Logger.h"
#include "SimpleBoundingBoxValidator.h"

#define LOG_TAG "ColorFilter"

using namespace smeyel;

ColorFilter::ColorFilter()
{
	this->defaultCollector = new DetectionBoundingBoxCollector( new SimpleBoundingBoxValidator() );
	this->collector = this->defaultCollector;
}

ColorFilter::~ColorFilter()
{
	delete this->defaultCollector;
	if (this->defaultCollector==this->collector)
	{
		this->collector = NULL;
	}
	this->defaultCollector = NULL;
}
