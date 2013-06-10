/*
 * CircleXYR.cpp
 *
 *  Created on: 2013.06.07.
 *      Author: norbi
 */

#include "CircleXYR.h"

using namespace cv;

CircleXYR::CircleXYR() : Vec3f()
{

}

CircleXYR::CircleXYR(const CircleXYR& theOther) : Vec3f()
{
	CircleXYR::operator =(theOther);
}

CircleXYR::CircleXYR(const Vec3f &theOther) : Vec3f(theOther)
{

}

CircleXYR& CircleXYR::operator =(const CircleXYR& theOther)
{
	if (this == &theOther)
		return *this;

	Vec3f::operator =(theOther);
	return *this;
}

CircleXYR& CircleXYR::operator=(const cv::Vec3f &theOther)
{
	if (this == &theOther)
		return *this;

	Vec3f::operator =(theOther);
	return *this;
}

float CircleXYR::getX() const
{
	return this->val[0];
}

float CircleXYR::getY() const
{
	return this->val[1];
}

float CircleXYR::getR() const
{
	return this->val[2];
}

SmartVector CircleXYR::getCenterPoint() const
{
	return SmartVector(this->getX(), this->getY());
}

