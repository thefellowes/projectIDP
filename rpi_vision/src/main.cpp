// ConsoleApplication2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include <vector>

using namespace cv;

RotatedRect find_marker(Mat &image, std::vector<int> &lowerArray, std::vector<int> &upperArray);
double distance_to_camera(double knownWidth, double focalLength, double perWidth);
std::string id_generator(char chars[]);
std::vector<std::string> appendWhereNoneString(std::vector<std::string> origin, std::vector<std::string> toAdd);
std::vector<int> appendWhereNoneInt(std::vector<int> &origin, std::vector<int> &toAdd);
double findFocalLength(Mat &img, std::vector<int> &lower, std::vector<int> &upper, double &width);

class Sliders {
public:
	std::string id_generator(int min, int max) {
		return std::to_string(min + (rand() % static_cast<int>(max - min + 1)));
	}

	template <typename T>
	void appendWhereNone(std::vector<T> &origin, std::vector<T> &toAdd) {
		for (size_t i = 0; i < toAdd.size(); i++) {
			if (origin.size() == i) {
				origin.push_back(toAdd[i]);
			}
		}
	}

	Sliders(std::string windowName = "", int amount = 6, std::string defaultSliderName = "", int defaultMinValue = 0, int defaultMaxValue = 100, bool newWindow = true, std::vector<std::string> sliderNames = {}, std::vector<int> minValues = {}, std::vector<int> maxValues = {}, std::vector<int> initValues = {}) {
		if (windowName == "") {
			windowName = id_generator(10000000, 99999999);
			newWindow = true;
		}
		if (newWindow) {
			namedWindow(windowName, 0);
		}

		this->windowName = windowName;
		this->sliderNames = sliderNames;
		this->amount = amount;

		defaultSliderName = defaultSliderName == "" ? id_generator(100000, 999999) : defaultSliderName;
		for (int i = 0; i < amount; i++)
		{
			if (i == sliderNames.size()) {
				sliderNames.push_back(defaultSliderName + " (" + std::to_string(i) + ")");
			}

			int minVal = i < minValues.size() ? minValues[i] : defaultMinValue;
			int maxVal = i < maxValues.size() ? maxValues[i] : defaultMaxValue;
			int initVal = i < initValues.size() ? initValues[i] : minVal;

			createTrackbar(sliderNames[i], windowName, &minVal, maxVal);
			setTrackbarPos(sliderNames[i], windowName, initVal);
		}

	}

	int val(int i) {
		if (i < amount && i >= 0) {
			return getTrackbarPos(sliderNames[i], windowName);
		}
		else {
			return -1;
		}
	}



private:
	std::string windowName;
	std::vector<std::string> sliderNames;
	int amount;

public:



};
class ColorIsolator {

	bool showControls;
	std::vector<int> initValues;
	std::string windowName;
	std::vector<std::string> sliderNames;
	Sliders sliders;
	std::vector<int> minVal = { 0, 0, 0 };
	std::vector<int> maxVal = { 255, 255, 255 };

public:
	ColorIsolator() : showControls(true), initValues(), windowName(""), sliderNames(), sliders(), minVal(), maxVal() {};
	ColorIsolator(std::vector<int> initValues1, std::string windowName1, std::vector<std::string> sliderNames1)
	{
		sliderNames = sliderNames1;
		initValues = initValues1;
		windowName = windowName1;
		showControls = true;
		std::vector<std::string> names = { "low r", "low g", "low b", "upp R", "upp G", "upp B" };
		std::vector<int> defaultInitValues = { 0, 0, 0, 255, 255, 255 };


		if (windowName == "")
		{
			char chars[26] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z' };
			windowName = id_generator(chars);
		}

		sliderNames = appendWhereNoneString(sliderNames, names);
		initValues = appendWhereNoneInt(initValues, defaultInitValues);
		sliders = Sliders(windowName, names.size(), "", 0, 255, true, sliderNames, {}, {}, initValues);
	}

	Mat isolate(Mat frame) {
		cvtColor(frame, frame, COLOR_BGR2HSV);

		std::vector<int> lower = { sliders.val(0), sliders.val(1), sliders.val(2) };
		std::vector<int> upper = { sliders.val(3), sliders.val(4), sliders.val(5) };

		Mat newImage;
		inRange(frame, lower, upper, newImage);
		return newImage;
	}

	std::vector<int> getLower()
	{
		std::vector<int> toReturn;
		toReturn.push_back(sliders.val(0));
		toReturn.push_back(sliders.val(1));
		toReturn.push_back(sliders.val(2));
		return toReturn;
	}

	std::vector<int> getUpper()
	{
		std::vector<int> toReturn;
		toReturn.push_back(sliders.val(3));
		toReturn.push_back(sliders.val(4));
		toReturn.push_back(sliders.val(5));
		return toReturn;
	}
};

int main()
{
	try {
		std::string IMAGE_PATHS[3] = { "C://Users//muize//Documents//school//project//lolxd.png", "C://Users//muize//Documents//school//project//above2.jpg", "C://Users//muize//Documents//school//project//lolxd3.jpg" };
		double width = 7.6;
		double KNOWN_DISTANCE = 7.6;
		double KNOWN_WIDTH = 7.6;
		std::vector<int> orangeInitValues = { 0, 186, 0, 188, 255, 192 };
		std::vector<int> yellowInitValues = { 24, 31, 156, 50, 133, 214 };
		std::vector<int> greenInitValues =	{ 56, 124, 29, 95, 255, 187 };
		std::vector<int> blueInitValues =	{ 88, 190, 43, 123, 255, 255 };
		std::vector<int> redInitValues =	{ 0, 186, 0, 188, 255, 192 };
		std::vector<int> blueMarkerValues = { 102, 73, 44, 141, 255, 255 };
		std::vector<std::string> names =	{ "low r", "low g", "low b", "upp R", "upp G", "upp B" };
		Mat pic = imread(IMAGE_PATHS[0]);
		Mat abovePic = imread(IMAGE_PATHS[1]);

		std::vector<int> lower = { 110, 50, 50 };
		std::vector<int> upper = { 130, 255, 255 };
		Scalar color(0, 255, 0);

		double focalLength = findFocalLength(pic, lower, upper, width);
		double focalLength2 = findFocalLength(abovePic, lower, upper, width);
		double cm = 0;
		ColorIsolator greenIsolator = ColorIsolator(blueMarkerValues, "green Controls", names);

		VideoCapture cap(0);
		if (!cap.isOpened())
			return -1;

		while (true)
		{
			Mat frame;
			cap >> frame;
			
			RotatedRect marker = find_marker(frame, greenIsolator.getLower(), greenIsolator.getUpper());
			Point2f vertices[4];
			marker.points(vertices);

			if (marker.size.area() != 0)
			{
				//if (marker.size.width / marker.size.height > 2.5)
				//{
					width = 7.6;
					cm = distance_to_camera(width, focalLength, marker.size.width);
					Point2f rect_points[4]; marker.points(rect_points);
					for (int i = 0; i < 4; i++) {
						line(frame, rect_points[i], rect_points[(i + 1) % 4], color, 1, 8);
					//}
				}
				/*else {
					width = 2.4;
					cm = distance_to_camera(width, focalLength2, marker.size.width);
					Point2f rect_points[4]; marker.points(rect_points);
					for (int i = 0; i < 4; i++) {
						line(frame, rect_points[i], rect_points[(i + 1) % 4], color, 1, 8);
					}
				}

				/*std::vector<std::vector<Point>> contoursTest = { contours1 };
				Scalar color(0, 255, 0);
				drawContours(image, contoursTest, -1, color, 2);*/
			}
			else {
				cm = 0;
			}

			putText(frame, std::to_string(cm),
			{frame.size().width - 200 , frame.size().height - 20}, FONT_HERSHEY_SIMPLEX, 2.0,
				color, 3);
			imshow("image", frame);
			imshow("greenimage", greenIsolator.isolate(frame));
			waitKey(1);
		}
	}
	catch (cv::Exception &e)
	{
		std::cout << e.msg << std::endl;
	}

	getchar();
}

double distance_to_camera(double knownWidth, double focalLength, double perWidth)
{
	return (knownWidth * focalLength) / perWidth;
}

std::string id_generator(char chars[])
{
		int randNum = rand() % (sizeof(chars) - 1);
		std::string s;
		s.push_back(chars[randNum]);
		return s;
}

std::vector<std::string> appendWhereNoneString(std::vector<std::string> origin, std::vector<std::string> toAdd)
{
	std::vector<std::string> returnOrigin = origin;
	for (int i = 0; i < toAdd.size(); i++)
	{
		if (returnOrigin.size() < toAdd.size())
		{
			returnOrigin.push_back(toAdd[i]);
		}
	}
	return returnOrigin;
}

double findFocalLength(Mat &img, std::vector<int> &lower, std::vector<int> &upper, double &width)
{
	double newWidth = width;
	double KNOWN_DISTANCE = 7.6;
	RotatedRect marker = find_marker(img, lower, upper);
	Point2f vertices[4];

	int w = marker.size.width;
	int h = marker.size.height;

	if (w > 0 && h > 0 && w > h) {
		if ((w / h) < 2) {
			newWidth = 2.4;
		}
	}

	if (w > 0 && h > 0 && h > w) {
		if ((h / w) < 2) {
			newWidth = 2.4;
		}
	}

	marker.points(vertices);
	double focalLength = (marker.size.height * KNOWN_DISTANCE) / newWidth;
	return focalLength;
}

std::vector<int> appendWhereNoneInt(std::vector<int> &origin, std::vector<int> &toAdd)
	{
		std::vector<int> returnOrigin = origin;
		for (int i = 0; i < toAdd.size(); i++)
		{
			if (returnOrigin.size() < toAdd.size())
			{
				returnOrigin.push_back(toAdd[i]);
			}
		}
		return returnOrigin;
	}

RotatedRect find_marker(Mat &image, std::vector<int> &lowerArray, std::vector<int> &upperArray)
{
	Mat canny_output;
	std::vector<std::vector<Point>> contours;
	std::vector<Point> contours1 = { {0,0} };
	std::vector<Vec4i> hierarchy;
	double KNOWN_WIDTH = 7.6;

	Mat gray, edged, hsv_img, frame_threshed, thresh;
	cvtColor(image, gray, COLOR_BGR2GRAY);
	GaussianBlur(gray, gray, { 5, 5 }, 0);
	Canny(gray, edged, 35, 125);
	cvtColor(image, hsv_img, COLOR_BGR2HSV);
	inRange(hsv_img, lowerArray, upperArray, frame_threshed);
	Mat imgray = frame_threshed;
	double ret = threshold(frame_threshed, thresh, 127, 255, 0);
	findContours(thresh, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	int maxArea = 500;

	for (auto c : contours)
	{
		if (contourArea(c) > maxArea) {
			contours1 = c;
			maxArea = contourArea(contours1);
		}
	}

	Rect points = boundingRect(contours1);
	int w = points.width;
	int h = points.height;
	int x = points.x;
	int y = points.y;
	int middleX = x + w / 2;
	int middleY = y + h / 2;


	if (h * w > 0)
	{
		if (h > w)
		{
			printf("standing\n");
			if ((h / w) >= 2)
			{
				if ((h / w) >= 3) {
					printf("long side, thin side\n");
				}
				else {
					printf("long side, thicc side\n");
				}
			}
			else {
				printf("short side\n");
			}
		}

		else {
			printf("laying\n");
			if ((w / h) >= 2)
			{
				if ((w / h) >= 3) {
					printf("long side, thin side\n");
				}
				else {
					printf("long side, thicc side\n");
				}
			}
			else {
				printf("short side\n");
			}
		}
	}

	

	circle(image, { middleX, middleY }, 10, (0, 0, 255), -1);
	imshow("testthing", image);

	return minAreaRect(contours1);	  
}


	

	

