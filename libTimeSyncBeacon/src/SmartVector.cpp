/*
 * SmartVector.cpp
 *
 *  Created on: 2013.06.05.
 *      Author: KOSZÓ, Norbert
 */

#include "SmartVector.h"
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;

SmartVector::SmartVector() : Matx21d()
{

}

SmartVector::SmartVector(const SmartVector & theOther) : Matx21d()
{
	operator =((Matx21d)theOther);
}

SmartVector::SmartVector(const cv::Matx21d& theOther) : Matx21d()
{
	operator =((Matx21d)theOther);
}

SmartVector::SmartVector(const cv::Point& pt) : Matx21d()
{
	operator =(pt);
}

SmartVector::SmartVector(const double x, const double y) : Matx21d()
{
	this->val[0] = x;
	this->val[1] = y;
}

SmartVector& SmartVector::operator=(const SmartVector& theOther)
{
	if (this == &theOther)
		return *this;

	operator =((Matx21d)theOther);
	return *this;
}

SmartVector& SmartVector::operator=(const Matx21d& theOther)
{
	if (this == &theOther)
		return *this;

	Matx21d::operator =(theOther);
	return *this;
}

SmartVector& SmartVector::operator=(const cv::Point& pt)
{
	this->val[0] = pt.x;
	this->val[1] = pt.y;
	return (*this);
}

Point SmartVector::toPoint() const
{
	Point retval;
	retval.x = cvRound(this->val[0]);
	retval.y = cvRound(this->val[1]);
	return retval;
}

double SmartVector::distance(const SmartVector& theOther, double pnorm) const
{
	double absSumPower = 0;

	//TODO check whether pnorm is invalid.

	for (size_t i = 0; i < 2; ++i) {
		absSumPower += pow(abs(this->val[i]-theOther.val[i]), pnorm);
	}
	return pow(absSumPower, 1/pnorm);
}

double SmartVector::getX() const
{
	return this->val[0];
}

double SmartVector::getY() const
{
	return this->val[1];
}
