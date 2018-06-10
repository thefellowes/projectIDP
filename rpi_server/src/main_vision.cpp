//ConsoleApplication2.cpp : Defines the entry point for the console application.
//

#include <string>
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
#include "functions.h"
#include <thread>

int main()
{
	std::vector<std::vector<int>> markerValues;
	markerValues.push_back({ 100, 73, 44, 141, 255, 255 }); //blue
	markerValues.push_back({ 34, 50, 50, 80, 220, 200 }); //green
	markerValues.push_back({ 20, 100, 100, 40, 255, 255 }); //yellow
	markerValues.push_back({ 6, 100, 100, 10, 255, 255 }); //orange
	markerValues.push_back({ 0, 100, 100, 5, 255, 255 }); //red
	markerValues.push_back({ 0, 0, 0, 30, 30, 30 });//black
	markerValues.push_back({225, 225, 225, 255, 255, 255})//white
	// for black
	//cv::inRange(imgHSV, cv::Scalar(0, 0, 0, 0), cv::Scalar(180, 255, 30, 0), imgThreshold);
	// for white   
	//cv::inRange(imgHSV, cv::Scalar(0, 0, 200, 0), cv::Scalar(180, 255, 255, 0), imgThreshold);



	cv::VideoCapture cap(0);
	if (!cap.isOpened())
		return -1;
	cv::Mat frame;
	cap.grab();
	int count = 0;
	cap.retrieve(frame);
	functions funct = functions(markerValues);
	
	while (true)
	{
			cap >> frame;
			//funct.find_marker_cup(frame);
			funct.update(frame);
			cv::imshow("image", frame);
			cv::waitKey(1);
	}
}