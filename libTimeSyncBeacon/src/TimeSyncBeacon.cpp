/*
 * TimeSyncBeacon.cpp
 *
 *  Created on: 2013.06.12.
 *      Author: norbi
 */
#include "TimeSyncBeacon.h"

TimeSyncBeacon::TimeSyncBeacon()
{
	this->tsbr = new TimeSyncBeaconReader();
	tsbr->setThreshold(220);
	this->tsbc = new TimeSyncBeaconConverter(255,10,10,100,10);
}

TimeSyncBeacon::~TimeSyncBeacon()
{
	delete this->tsbc;
	delete this->tsbr;
}


long long TimeSyncBeacon::GetTimeUsFromImage(const cv::Mat &sourceImage)
{
	tsbr->ProcessImage(sourceImage);
	return tsbc->GetTimeUsFromBrightness(tsbr->davidArray);
}
