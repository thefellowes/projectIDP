//ConsoleApplication2.cpp : Defines the entry point for the console application.
//

#include <string>
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
#include "Sliders.hpp"
#include "ColorIsolator.hpp"
#include "functions.h"

using namespace cv;

int main()
{
	std::vector<std::vector<int>> markerValues;
	markerValues.push_back({ 100, 73, 44, 141, 255, 255 }); //blue
	markerValues.push_back({ 34, 50, 50, 80, 220, 200 }); //green
	markerValues.push_back({ 20, 100, 100, 40, 255, 255 }); //yellow
	markerValues.push_back({ 6, 100, 100, 10, 255, 255 }); //orange
	markerValues.push_back({ 0, 100, 100, 5, 255, 255 }); //red
	
	VideoCapture cap(0);
	if (!cap.isOpened())
		return -1;
	Mat frame;
	cap.grab();
	cap.retrieve(frame);
	functions funct = functions(markerValues);
	
	while (true)
	{
		cap >> frame;	
		funct.update(frame);
		frame = funct.getImage();
		waitKey(1);
	}
}