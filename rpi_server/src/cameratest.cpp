// ConsoleApplication2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include "opencv2/opencv.hpp"
#include <raspicam-0.1.6/src/raspicam_cv.h>
#include <iostream>
#include <vector>

using namespace cv;

int main()
{
	raspicam::RaspiCam_Cv cap;
	if (!cap.isOpened())
		return -1;
	Mat frame;

	while (true)
	{
		cap.grab();
		cap.retrieve(frame);
		imshow("image", frame);
		waitKey(1);
	}
}