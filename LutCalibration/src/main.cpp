#include <iostream>	// for standard I/O
#include <string>   // for strings

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write

using namespace std;
using namespace cv;

class LutColor
{
	unsigned char *rValues;
	unsigned char *gValues;
	unsigned char *bValues;
	unsigned int stepsPerChannel;

	/** min/max values -> ...Values arrays */
	void initColorValues()
	{
		float dR = (float)(Rmax-Rmin)/(float)(this->stepsPerChannel);
		float dG = (float)(Gmax-Gmin)/(float)(this->stepsPerChannel);
		float dB = (float)(Bmax-Bmin)/(float)(this->stepsPerChannel);
		for(int step=0; step<this->stepsPerChannel; step++)
		{
			rValues[step] = (int)(Rmin + dR*(float)step);
			gValues[step] = (int)(Gmin + dG*(float)step);
			bValues[step] = (int)(Bmin + dB*(float)step);
		}
	}

	/**
		@warning Do not forget to call initColorValues() before using this!
	*/
	Scalar getColorForIdx(unsigned int row, unsigned int col)
	{
		unsigned int subRectIdx = row*6+col;
		if (subRectIdx<27)
		{
			unsigned int rIdx = subRectIdx / 9;
			unsigned int gIdx = (subRectIdx % 9) / 3;
			unsigned int bIdx = subRectIdx % 3;

			return Scalar(rValues[rIdx],gValues[gIdx],bValues[bIdx]);
		}
		// Remaining areas get principal color
		return Scalar(rValues[1],gValues[1],bValues[1]);
	}

	void adjustToCoverColor(unsigned char r, unsigned char g, unsigned char b)
	{
		// TODO adjust minimal and maximal values, or directly a given LUT
		// Should not support only cubes in the color space!
		if (r>Rmax)	Rmax=r;
		if (r<Rmin)	Rmin=r;
		if (g>Gmax)	Gmax=g;
		if (g<Gmin)	Gmin=g;
		if (b>Bmax)	Bmax=b;
		if (b<Bmin)	Bmin=b;
	}

public:
	// The exact color (also used in InverseLut)
	unsigned char R, G, B;
	// Boundaries
	unsigned char Rmin, Rmax;
	unsigned char Gmin, Gmax;
	unsigned char Bmin, Bmax;

	LutColor(int aStepsPerChannel=3)
	{
		this->stepsPerChannel = aStepsPerChannel;
		rValues = new unsigned char[aStepsPerChannel];
		gValues = new unsigned char[aStepsPerChannel];
		bValues = new unsigned char[aStepsPerChannel];
	}

	~LutColor()
	{
		delete rValues;
		delete gValues;
		delete bValues;
	}

	void set(	unsigned char aRmin, unsigned char aR, unsigned char aRmax,
				unsigned char aGmin, unsigned char aG, unsigned char aGmax, 
				unsigned char aBmin, unsigned char aB, unsigned char aBmax)
	{
		this->Rmin	= aRmin;
		this->R		= aR;
		this->Rmax	= aRmax;
		this->Gmin	= aGmin;
		this->G		= aG;
		this->Gmax	= aGmax;
		this->Bmin	= aBmin;
		this->B		= aB;
		this->Bmax	= aBmax;
	}

	void drawCalibrationRect(Mat &img, const Rect &targetArea)
	{
		initColorValues();

		Size2f subAreaSize;
		// Size is 5x6 +1 unit border in every direction
		subAreaSize.width = targetArea.width / 7.0F;
		subAreaSize.height = targetArea.height / 8.0F;

		rectangle(img,targetArea,Scalar(R,G,B),CV_FILLED);
		for(unsigned int row=0; row<6; row++)
		{
			for(unsigned int col=0; col<5; col++)
			{
				Rect subRect;
				subRect.x = targetArea.x + (col+1.0F)*subAreaSize.width;
				subRect.y = targetArea.y + (row+1.0F)*subAreaSize.height;
				subRect.width = subAreaSize.width;
				subRect.height = subAreaSize.height;
				rectangle(img,subRect,getColorForIdx(row,col),CV_FILLED);
			}
		}
	}

	void readCalibrationRect(Mat &img, const Rect &targetArea)
	{
		assert(img.type()==CV_8UC3);
		// Go along every pixel and adjust minimal and maximal values accordingly
		// TODO: what about quantizing gaps causing holes in the LUT?
		for(unsigned int row=targetArea.y; row<targetArea.y+targetArea.height; row++)
		{
			for(unsigned int col=targetArea.x; col<targetArea.x+targetArea.width; col++)
			{
				// BGR image
				unsigned char r = img.at<uchar>(row,col,2);
				unsigned char g = img.at<uchar>(row,col,1);
				unsigned char b = img.at<uchar>(row,col,0);
				adjustToCoverColor(r,g,b);
			}
		}
	}
};

class LutCalibrationImage
{
public:
	vector<LutColor*> lutColors;	// Should have 6 elements right now...

	void initDefaultLutColors()
	{
		LutColor *BLK=new LutColor(3);
		LutColor *WHT=new LutColor(3);
		LutColor *RED=new LutColor(3);
		LutColor *GRN=new LutColor(3);
		LutColor *BLU=new LutColor(3);
		LutColor *DMY=new LutColor(3);	// DMY is Dummy
		BLK->set(0,16,32,		0,16,32,		0,16,32);
		WHT->set(224,240,255,	224,240,255,	224,240,255);
		RED->set(224,240,255,	0,16,32,		0,16,32);
		GRN->set(0,16,32,		224,240,255,	0,16,32);
		BLU->set(0,16,32,		0,16,32,		224,240,255);
		DMY->set(112,128,140,	112,128,140,	112,128,140);
		lutColors.push_back(BLK);
		lutColors.push_back(WHT);
		lutColors.push_back(DMY);
		lutColors.push_back(RED);
		lutColors.push_back(GRN);
		lutColors.push_back(BLU);
	}

	Mat generateCalibrationImage(int sizeFactor)
	{
		assert(lutColors.size()==6);

		const Scalar MarkerColor(0,0,0);

		Mat img(sizeFactor*11,sizeFactor*14,CV_8UC3);
		img.setTo(Scalar(255,255,255));

		// Draw markers
		for(int i=0; i<3; i++)
		{
			Point2f leftCircleCenter(sizeFactor/2.0,sizeFactor*i*5+sizeFactor/2.0);
			circle(img,leftCircleCenter,sizeFactor/2,MarkerColor,CV_FILLED);
			Point2f rightCircleCenter(13*sizeFactor+sizeFactor/2.0,sizeFactor*i*5+sizeFactor/2.0);
			circle(img,rightCircleCenter,sizeFactor/2,MarkerColor,CV_FILLED);
		}

		// Draw color calibration rectangles
		for(int row=0; row<2; row++)
		{
			for(int col=0; col<3; col++)
			{
				LutColor *lutColor=lutColors[row*3+col];
				Rect targetArea(sizeFactor*(1+col*4),sizeFactor*(1+row*5),sizeFactor*4,sizeFactor*4);
				lutColor->drawCalibrationRect(img,targetArea);
			}
		}
		return img;
	}

	void ReadCalibrationImage(Mat &img)
	{
		// Find 6 markers

		// Calculate homography

		// Undistort image

		// Read calibration rectangles

	}
};



int main(int argc, char *argv[], char *window_name)
{
	namedWindow("IMG", CV_WINDOW_AUTOSIZE);

	LutCalibrationImage lutCalibrationImage;

	lutCalibrationImage.initDefaultLutColors();
	Mat img = lutCalibrationImage.generateCalibrationImage(50);


	while(true) //Show the image captured in the window and repeat
	{
		imshow("IMG",img);

		char ch = waitKey(25);
		if (ch==27)
		{
			break;
		}
		else if (ch=='s')
		{
			cvSaveImage("LutCalibrationImage.png",&img);
			break;
		}
	}
	return 0;
}
