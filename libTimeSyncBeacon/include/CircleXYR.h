/*
 * CircleXYR.h
 *
 *  Created on: 2013.06.07.
 *      Author: norbi
 */

#ifndef CIRCLEXYR_H_
#define CIRCLEXYR_H_

#include <opencv2/core/core.hpp>
#include "SmartVector.h"


class CircleXYR : public cv::Vec3f {
public:
	CircleXYR();
	CircleXYR(const CircleXYR &theOther);
	CircleXYR(const cv::Vec3f &theOther);
	CircleXYR & operator=(const CircleXYR &theOther);
	CircleXYR & operator=(const cv::Vec3f &theOther);
	float getX() const;
	float getY() const;
	float getR() const;
	SmartVector getCenterPoint() const;
};


#endif /* CIRCLEXYR_H_ */
