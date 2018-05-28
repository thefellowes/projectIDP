#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include "ColorIsolator.h"
#include "Sliders.h"
#include "functions.h"

using namespace cv;

	void functions::setCoordinates(int x, int y, int w, int h, int middleX, int middle_Y)
	{
		this->middleX = middleX;
		this->x = x;
		this->y = y;
		this->h = h;
		this->w = w;
	}

	functions::functions(std::vector<int> initValues = {})
	{
		std::vector<std::string> names = { "low r", "low g", "low b", "upp R", "upp G", "upp B" };
		isolator = ColorIsolator(initValues, "green Controls", names);
	}

	Mat functions::getImage()
	{
		return image;
	}

	void functions::update(Mat image_)
	{
		image = image_;
		marker = find_marker(image, isolator.getLower(), isolator.getUpper());
		Point2f rect_points[4]; marker.points(rect_points);
		Scalar color(0, 255, 0);
		for (int i = 0; i < 4; i++) {
			line(image, rect_points[i], rect_points[(i + 1) % 4], color, 1, 8);
		}
		imshow("image", image);
		imshow("greenimage", isolator.isolate(image));

	}

	std::string functions::getStance()
	{
		std::string returnString = "";
		if (h * w > 0)
		{
			if (h > w)
			{
				returnString.append("standing ");
				if ((h / w) >= 2)
				{
					if ((h / w) >= 3) {
						returnString.append("long side, thin side ");
					}
					else {
						returnString.append("long side, thicc side ");
					}
				}
				else {
					returnString.append("short side ");
				}
			}

			else {
				returnString.append("laying ");
				if ((w / h) >= 2)
				{
					if ((w / h) >= 3) {
						returnString.append("long side, thin side ");
					}
					else {
						returnString.append("long side, thicc side ");
					}
				}
				else {
					returnString.append("short side ");
				}
			}
		}
		return returnString;
	}
	RotatedRect functions::find_marker(Mat image, std::vector<int> lowerArray, std::vector<int> upperArray)
	{
		Mat canny_output;
		std::vector<std::vector<Point>> contours;
		std::vector<Point> contours1 = { { 0,0 } };
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
		setCoordinates(x, y, w, h, middleX, middleY);
	
		circle(image, { middleX, middleY }, (w + h) * 0.05, (0, 0, 255), -1);
		imshow("testthing", image);

		return minAreaRect(contours1);
	}


