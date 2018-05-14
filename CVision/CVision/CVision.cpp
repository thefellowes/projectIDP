// CVision.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "opencv2\opencv.hpp"
#include "ColorIsolator.hpp"
#include "RobotArm.hpp"

using namespace cv;


int main(int argv, char** argc)
{
	//RobotArm arm = RobotArm(5.0, 4.0);
	////std::vector<std::vector<float>> path = arm.getPath(-3.56f, 6.43f, -3.56f, 7.43f, 90.0f, 90.0f);
	//std::vector<std::vector<float>> path = arm.getPath(-2.0f, 4.0f, -6.0f, 8.0f, 0.0f, -90.0f);
	////std::vector<std::vector<float>> path = arm.getBezierPath(5.0f, 5.0f, 5.0f, 10.0f, 0.0f, 0.0f);
	////std::vector<float> angles = arm.PosToAngles(-3.56f, 6.43f, 90.0f);
	//std::vector<float> angles = arm.PosToAngles(-5.24, 7.96, -81);
	//for (size_t i = 0; i < path.size(); i++)
	//{
	//	std::cout << "1.   = " << path[i][0] << std::endl;
	//	std::cout << "2..  = " << path[i][1] << std::endl;
	//	std::cout << "3... = " << path[i][2] << std::endl;
	//}
	//std::cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << std::endl;
	//path = arm.PosToAngles(path);
	//for (size_t i = 0; i < path.size(); i++)
	//{
	//	std::cout << "1.   = " << path[i][0] << std::endl;
	//	std::cout << "2..  = " << path[i][1] << std::endl;
	//	std::cout << "3... = " << path[i][2] << std::endl;
	//}

	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return -1;

	Mat edges;
	namedWindow("frame", 1);
	ColorIsolator isolator(true, { 10, 20, 30, 70 }, "isolator Controls");
	while (true)
	{
		Mat frame;
		cap >> frame; // get a new frame from camera
		
		Mat mask = isolator.getMask(frame);
		Mat filtered;
		bitwise_and(frame, frame, filtered, mask);
		imshow("mask", mask);
		imshow("filtered", filtered);

		if (waitKey(30) >= 0) break;
	}
	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}

