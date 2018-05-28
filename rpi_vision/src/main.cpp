// ConsoleApplication2.cpp : Defines the entry point for the console application.
//

#include <string>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "Sliders.h"
#include "ColorIsolator.h"
#include "functions.h"

using namespace cv;

int main()
{
	std::vector<int> blueMarkerValues = { 102, 73, 44, 141, 255, 255 };
	VideoCapture cap(0);
	if (!cap.isOpened())
		return -1;
	Mat frame;
	cap >> frame;
	functions funct = functions(blueMarkerValues);
	while (true)
	{
		cap >> frame;
		funct.update(frame);
		frame = funct.getImage();
		waitKey(1);
	}
}
