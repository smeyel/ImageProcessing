#include "DefaultColorCodes.h"

#include "LutCalibrationPattern.h"

using namespace smeyel;

int LutCalibrationPattern::findMinIdx(vector<Vec3f> marks, float coeffX, float coeffY, float offset)
{
	int minIdx = 0;
	float minValue = coeffX*marks[0][0] + coeffY*marks[0][1] + offset;
	for(int i=1; i<marks.size(); i++)
	{
		float currentValue = coeffX*marks[i][0] + coeffY*marks[i][1] + offset;
		if (currentValue < minValue)
		{
			minIdx = i;
			minValue = currentValue;
		}
	}
	return minIdx;
}

// used by findInImage
void LutCalibrationPattern::setFromCircleVector(vector<Vec3f> circles)
{
	Vec3f marks[4];
	// Reoder the circles in predefined order (upper left, upper right, lower left, lower right)
	int idx = findMinIdx(circles,1,1,0);	// arg min ( x + y )
	marks[0] = circles[idx];
	idx = findMinIdx(circles,-1,1,640);	// arg min ( 640-x + y )
	marks[1] = circles[idx];
	idx = findMinIdx(circles,1,-1,480);	// arg min ( x + 480-y )
	marks[2] = circles[idx];
	idx = findMinIdx(circles,-1,-1,1120);	// arg min ( 640-x + 480-y )
	marks[3] = circles[idx];

	corners[0] = Point2f( marks[0][0]+marks[0][2],marks[0][1]+marks[0][2] );
	corners[1] = Point2f( marks[1][0]-marks[1][2],marks[1][1]+marks[1][2] );
	corners[2] = Point2f( marks[2][0]+marks[2][2],marks[2][1]-marks[2][2] );
	corners[3] = Point2f( marks[3][0]-marks[3][2],marks[3][1]-marks[3][2] );
}

void LutCalibrationPattern::createColorCodeMask(Mat &dst)
{
	OPENCV_ASSERT(dst.type()==CV_8UC1, "createColorCodeMask", "ColorCode mask target type is not CV_8UC1.");
	const unsigned char maskSkipValue = COLORCODE_NONE;
	dst.setTo(maskSkipValue);
	for(int row=0; row<2; row++)
	{
		for(int col=0; col<3; col++)
		{
			Tetragon subarea;
			getSubArea(row,col,subarea);
			unsigned char colorCodes[] = { COLORCODE_BLK, COLORCODE_WHT, maskSkipValue, COLORCODE_RED, COLORCODE_GRN, COLORCODE_BLK };
//				unsigned char colorCodes[] = { COLORCODE_BLK, COLORCODE_WHT, maskSkipValue, COLORCODE_RED, COLORCODE_GRN, COLORCODE_BLU };
//				unsigned char colorCodes[] = { COLORCODE_BLK, COLORCODE_WHT, maskSkipValue, COLORCODE_RED, COLORCODE_GRN, maskSkipValue };
			subarea.addCodedMask(dst,colorCodes[row*3+col]);
		}
	}
}

bool LutCalibrationPattern::findInImage(Mat &src)
{
	OPENCV_ASSERT(src.cols==640 && src.rows==480,"CalibrateLut", "Source image is not 640x480...");
	OPENCV_ASSERT(src.type() == CV_8UC3,"CalibrateLut", "Source image is not CV_8UC3...");
	// Find circles
	Mat grayImage;
	cvtColor(src, grayImage, CV_BGR2GRAY);
	GaussianBlur( grayImage, grayImage, Size(9, 9), 2, 2 );
	vector<Vec3f> circles;
	HoughCircles(grayImage, circles, CV_HOUGH_GRADIENT,
					1,	// full recolution accumulator
					grayImage.rows/4,	// minimal distance between the centers of detected circles
					100,25);
	for( size_t i = 0; i < circles.size(); i++ )
	{
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
			// draw the circle center
//			 circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
			// draw the circle outline
//			 circle( src, center, radius, Scalar(0,0,255), 3, 8, 0 );
	}

	if (circles.size() != 4)
	{
		//cout << "Not 4 circles were found... (" << circles.size() << ")" << endl;
		return false;
	}

	setFromCircleVector(circles);
	return true;
}

void LutCalibrationPattern::getSubArea(int row, int col, Tetragon &subArea)
{
	unitHorizontal.x = (corners[1].x - corners[0].x) / 21.0F;
	unitHorizontal.y = (corners[1].y - corners[0].y) / 21.0F;

	unitVertical.x = (corners[2].x - corners[0].x) / 18.0F;
	unitVertical.y = (corners[2].y - corners[0].y) / 18.0F;

	// Get upper left corner
	Point2f UL = this->corners[0] + this->unitHorizontal + this->unitVertical
		+ col*7*this->unitHorizontal + row*10*this->unitVertical;
	Point2f UR = UL + 5*this->unitHorizontal;
	Point2f LL = UL + 6*this->unitVertical;
	Point2f LR = LL + 5*this->unitHorizontal;

	subArea.corners[0] = UL;
	subArea.corners[1] = UR;
	subArea.corners[2] = LL;
	subArea.corners[3] = LR;
}

// helper to wrap getSubArea to result polyline instead of RectangularArea.
void LutCalibrationPattern::getPolylineForSubArea(int row, int col, Point *targetOf4Points)
{
	Tetragon subarea;
	getSubArea(row,col,subarea);
	for(int i=0; i<4; i++)
	{
		targetOf4Points[i].x = cvRound(subarea.corners[i].x);
		targetOf4Points[i].y = cvRound(subarea.corners[i].y);
	}
}

void LutCalibrationPattern::updateLUT(LutColorFilter &lut, Mat &src)
{
	// Create color code mask
	Mat colorCodeMask(src.rows, src.cols, CV_8UC1);

	createColorCodeMask(colorCodeMask);

	/*Mat visLut(src.rows, src.cols, CV_8UC3);
	lut.InverseLut(colorCodeMask,visLut);
	imshow("LUT adapt mask (vis)",visLut);
	imshow("LUT adapt src",src);
	waitKey(0);*/

	lut.ExtendLutToConformMask(src,colorCodeMask,COLORCODE_NONE);
}
