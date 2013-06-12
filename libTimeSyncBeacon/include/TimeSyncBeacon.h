/*
 * TimeSyncBeacon.h
 *
 *  Created on: 2013.06.12.
 *      Author: norbi
 */

#ifndef TIMESYNCBEACON_H_
#define TIMESYNCBEACON_H_
#include "TimeSyncBeaconConverter.h"
#include "TimeSyncBeaconReader.h"
#include <opencv2/core/core.hpp>


class TimeSyncBeacon {
private:
	TimeSyncBeaconReader* tsbr;
	TimeSyncBeaconConverter* tsbc;
public:

	TimeSyncBeacon();
	~TimeSyncBeacon();
	long long GetTimeUsFromImage(const cv::Mat &sourceImage);
};


#endif /* TIMESYNCBEACON_H_ */
