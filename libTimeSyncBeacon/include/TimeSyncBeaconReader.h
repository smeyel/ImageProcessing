/*
 * TimeSyncBeaconReader.h
 *
 *  Created on: 2013.06.08.
 *      Author: norbi
 */

#ifndef TIMESYNCBEACONREADER_H_
#define TIMESYNCBEACONREADER_H_

#ifdef _WIN32
typedef unsigned char uint8_t;
#endif

#include <opencv2/core/core.hpp>
#include "SmartVector.h"
#include "CircleXYR.h"

class TimeSyncBeaconReader {
private:
	unsigned char thres;
	unsigned theoreticalLedSize;
	SmartVector corners[4];
	std::vector<CircleXYR> circles;
	std::vector<SmartVector> LEDcenterPoints;
	int LEDcountRow;
	cv::Mat sourceImage;

	void preprocessImage();
	void findCorners();
	cv::Rect RectangleFromCorners(const SmartVector corners[4]);
	void GeneratePoints(const SmartVector &a, const SmartVector &b);
	void GenerateLEDcenterPoints();
	void CreateDavidArray();
	unsigned satSub(int a, int b);



public:
	uint8_t davidArray[64];
	cv::Mat channel[3];

	unsigned char getTreshold();
	void setThreshold(unsigned char thres);

	TimeSyncBeaconReader();
	void ProcessImage(cv::Mat srcImg);
	void GenerateImage();
	cv::Rect getBoundingRect();

};


#endif /* TIMESYNCBEACONREADER_H_ */
