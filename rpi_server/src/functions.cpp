#include "opencv2\opencv.hpp"
#include <vector>
#include <string>
#include "ColorIsolator.hpp"
#include "Sliders.hpp"
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

	functions::functions(std::vector<std::vector<int>> initValues = {})
	{
		std::vector<std::string> names = { "low r", "low g", "low b", "upp R", "upp G", "upp B" };
		isolators.push_back(ColorIsolator(initValues[0]));
		isolators.push_back(ColorIsolator(initValues[1]));
		isolators.push_back(ColorIsolator(initValues[2]));
		isolators.push_back(ColorIsolator(initValues[3]));
		isolators.push_back(ColorIsolator(initValues[4]));
		colorNames.push_back("blue");
		colorNames.push_back("green");
		colorNames.push_back("yellow");
		colorNames.push_back("orange");
		colorNames.push_back("red");
		markers.push_back(RotatedRect(Point2f(0, 0), Size2f(0, 0), 0));
		markers.push_back(RotatedRect(Point2f(0, 0), Size2f(0, 0), 0));		
		markers.push_back(RotatedRect(Point2f(0, 0), Size2f(0, 0), 0));
		markers.push_back(RotatedRect(Point2f(0, 0), Size2f(0, 0), 0));
		markers.push_back(RotatedRect(Point2f(0, 0), Size2f(0, 0), 0));
	}

	Mat functions::getImage()
	{
		return image;
	}

	void functions::update(Mat image_)
	{
		image = image_;
		updateMarkers();
		imshow("image", image);
		//imshow("greenimage", isolator.isolate(image));
	}

	void functions::updateMarkers()
	{
		int biggestY = 100000000;
		Point2f rect_points_index[4];
		Scalar color(0, 255, 0);
		for (int i = 0; i < 5; i++)
		{
			markers[i] = find_marker(image, isolators[i].getLower(), isolators[i].getUpper(), colorNames[i]);
			Point2f rect_points[4];
			markers[i].points(rect_points);
			
			if (rect_points[0].y < biggestY && rect_points[0].y > 0)
			{
				biggestY = rect_points[0].y;
				for (int j = 0; j < 4; j++)
				{
					rect_points_index[j] = rect_points[j];
				}
			}
		}

		for (int k = 0; k < 4; k++)
		{
			line(image, rect_points_index[k], rect_points_index[(k + 1) % 4], color, 1, 8);
		}

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
	RotatedRect functions::find_marker(Mat image, std::vector<int> lowerArray, std::vector<int> upperArray, std::string colorName)
	{
			Mat canny_output;
			std::vector<std::vector<Point>> contours;
			std::vector<Point> contours1 = { { 0,0 } };
			std::vector<Vec4i> hierarchy;

			Mat gray, edged, hsv_img, frame_threshed, thresh;
			cvtColor(image, gray, COLOR_BGR2GRAY);
			GaussianBlur(gray, gray, { 5, 5 }, 0);
			Canny(gray, edged, 35, 125);
			cvtColor(image, hsv_img, COLOR_BGR2HSV);
			inRange(hsv_img, lowerArray, upperArray, frame_threshed);
			Mat imgray = frame_threshed;
			double ret = threshold(frame_threshed, thresh, 127, 255, 0);
			findContours(thresh, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
			int minArea = 1000;
			int maxArea = 30000;

			for (auto c : contours)
			{
				if (contourArea(c) > minArea && contourArea(c) < maxArea) {
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

			putText(image, colorName, { middleX, middleY }, FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 255, 255));
			circle(image, { middleX, middleY }, (w + h) * 0.05, (0, 0, 255), -1);
			return minAreaRect(contours1);
	}

	void functions::find_marker_cup(Mat image)
	{
		Mat gray;
		cvtColor(image, gray, CV_BGR2GRAY);

		// Reduce the noise so we avoid false circle detection
		GaussianBlur(gray, gray, Size(9, 9), 2, 2);

		std::vector<Vec3f> circles;

		// Apply the Hough Transform to find the circles
		HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, 30, 200, 50, 0, 0);
		int maxArea = 500;

		// Draw the circles detected
		for (size_t i = 0; i < circles.size(); i++)
		{
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
			circle(image, center, 3, Scalar(0, 255, 0), -1, 8, 0);// circle center     
			circle(image, center, radius, Scalar(0, 0, 255), 3, 8, 0);// circle outline
			std::cout << "center : " << center << "\nradius : " << radius << std::endl;
		}

		// Show your results
		namedWindow("Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE);
		imshow("Hough Circle Transform Demo", image);
	}

