/*
 * SmartVector.h
 *
 *  Created on: 2013.06.05.
 *      Author: KOSZÓ, Norbert
 */

#ifndef SmartVector_H_
#define SmartVector_H_

#include <opencv2/core/core.hpp>


class SmartVector : public cv::Matx21d {
public:

	SmartVector();
	SmartVector(const SmartVector& theOther);
	SmartVector(const cv::Matx21d& theOther);
	SmartVector(const cv::Point& pt);
	SmartVector(const double x, const double y);
	SmartVector& operator=(const SmartVector& theOther);
	SmartVector& operator=(const cv::Matx21d& theOther);
	SmartVector& operator=(const cv::Point& pt);

	cv::Point toPoint() const;

	double distance(const SmartVector& theOther, double pnorm = 1) const;
	double getX() const;
	double getY() const;
};


#endif /* SmartVector_H_ */
