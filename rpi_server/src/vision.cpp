#include "opencv2/opencv.hpp"
#include <vector>
#include <string>
#include "vision.h"
#include <thread>
#include <chrono>
#include <numeric>

#include <iostream>

Vision::Vision(std::vector<std::vector<int>> initValues = {})
{
	doUpdateFrame = false;

	std::vector<std::string> names = { "low r", "low g", "low b", "upp R", "upp G", "upp B" };
	for (int i = 0; i < 5; i++)
	{
		std::vector<int> lower, upper;
		for (int j = 0; j < 3; j++)
		{
			lower.push_back(initValues[i][j]);
		}
		for (int k = 3; k < 6; k++)
		{
			upper.push_back(initValues[i][k]);
		}

		lowerArrays.push_back(lower);
		upperArrays.push_back(upper);
		markers.push_back(cv::RotatedRect(cv::Point2f(0, 0), cv::Size2f(0, 0), 0));
	}

	colorNames.push_back("blue");
	colorNames.push_back("green");
	colorNames.push_back("yellow");
	colorNames.push_back("orange");
	colorNames.push_back("red");

	programNumber = -1;

	mutex = new std::mutex();
}

int Vision::startVision()
{
	isActive = true;

	cv::VideoCapture cap(0);
	if (!cap.isOpened())
		return -1;
	cv::Mat frame;
	cap.grab();
	cap.retrieve(frame);

	while(isActive)
	{
		while (doUpdateFrame) {
			if (cap.read(frame)) {
				mutex->lock();
				image = frame;
				mutex->unlock();
			}
			//cap >> frame;
			//image = frame;
			//switch (programNumber) {
			//case 1:
			//	find_marker_cup();
			//	break;
			//case 2:
			//	find_waitPoint();
			//	find_line();
			//	break;
			//default:
			//update(frame);
			//	break;
			//}

			//cv::imshow("image", frame);
			//cv::waitKey(1);
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
		
		cv::waitKey(1);
	}

}

void Vision::stopVision() {
	isActive = false;
}

void Vision::setProgram(int number)
{
	programNumber = number;
}

cv::Mat Vision::getImage()
{
	return image;
}

void Vision::update(cv::Mat image_)
{
	image = image_;
	updateMarkers();
}

void Vision::updateMarkers()
{
	int biggestY = 100000000;
	cv::Point2f rect_points_index[4];
	cv::Scalar color(0, 255, 0);
	find_markers();
	for (int i = 0; i < 5; i++)
	{
		cv::Point2f rect_points[4];
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
		cv::line(image, rect_points_index[k], rect_points_index[(k + 1) % 4], color, 1, 8);
	}

}

std::string Vision::getStance(int h, int w)
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

void Vision::find_markers()
{
	std::vector<std::thread> color_pool;
	size_t i = 0;
	while (i < lowerArrays.size())
	{
		color_pool.push_back(std::thread(&Vision::find_marker_by_color, this, std::ref(i), std::ref(image)));
		i++;
	}

	i = 0;
	while (i < lowerArrays.size())
	{
		color_pool[i].join();
		i++;
	}

}

//void Vision::find_marker_by_color(int i)
bool Vision::find_marker_by_color(int i, cv::Mat img)
{
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Point> contours1 = { { 0,0 } };
	std::vector<cv::Vec4i> hierarchy;
	int minArea = 3000;
	int maxArea = 30000;
	cv::Mat gray, edged, hsv_img, frame_threshed, thresh;
	cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
	cv::GaussianBlur(gray, gray, { 5, 5 }, 0);
	//cv::Canny(gray, edged, 35, 125);
	cv::cvtColor(img, hsv_img, cv::COLOR_BGR2HSV);
	cv::inRange(hsv_img, lowerArrays[i], upperArrays[i], frame_threshed);
	//double ret = cv::threshold(frame_threshed, thresh, 127, 255, 0);
	cv::findContours(thresh, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	for (auto c : contours)
	{
		if (cv::contourArea(c) > minArea && cv::contourArea(c) < maxArea)
		{
			contours1 = c;
			minArea = cv::contourArea(contours1);
		}
	}

	if (minArea > 3000)
	{
		cv::Rect points = boundingRect(contours1);
		int w = points.width;
		int h = points.height;
		int x = points.x;
		int y = points.y;
		int middleX = x + w / 2;
		int middleY = y + h / 2;
		cv::putText(img, colorNames[i], { middleX, middleY }, cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255, 255));
		cv::circle(img, { middleX, middleY }, (w + h) * 0.05, (0, 0, 255), -1);
		return true;
	}
	
	//minArea = 3000;
	markers[i] = cv::minAreaRect(contours1);
	//cv::imshow("img", img);
	//cv::imshow("hsv_img", hsv_img);
	return false;
}

//void Vision::find_marker_cup()
char Vision::find_marker_cup()
{
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Point> contours1 = { { 0,0 } };
	std::vector<cv::Vec4i> hierarchy;
	int minY = 0;
	int minArea = 750;
	int maxArea = 30000;
	cv::Mat gray, edged, hsv_img, frame_threshed, thresh;
	cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
	cv::GaussianBlur(gray, gray, { 5, 5 }, 0);
	cv::cvtColor(image, hsv_img, cv::COLOR_BGR2HSV);
	cv::inRange(hsv_img, lowerArrays[1], upperArrays[1], frame_threshed);
	double ret = cv::threshold(frame_threshed, thresh, 127, 255, 0);
	cv::findContours(thresh, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	for (auto c : contours)
	{
		if (c.at(0).y > minY && cv::contourArea(c) > minArea && cv::contourArea(c) < maxArea)
		{
			contours1 = c;
			minY = c.at(0).y;
		}
	}

	if (minY > 0)
	{
		cv::Rect points = boundingRect(contours1);
		int w = points.width;
		int h = points.height;
		int x = points.x;
		int y = points.y;
		int middleX = x + w / 2;
		int middleY = y + h / 2;
		cv::putText(image, colorNames[1], { middleX, middleY }, cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255, 255));
		cv::circle(image, { middleX, middleY }, (w + h) * 0.05, (0, 0, 255), -1);

		if (cv::contourArea(contours1) > 8000)
		{
			return 'm';
		}

		if (cv::contourArea(contours1) <= 8000) {
			if (x < 80)
			{
				std::cout << "Move right" << std::endl;
				return 'r';
			}

			else if (x > 450)
			{
				std::cout << "Move left" << std::endl;
				return 'l';
			}

			else {
				std::cout << "Move forward" << std::endl;
				return 'f';

			}
		}
	}
	else {
		std::cout << "Rotating to find cup" << std::endl;
		return 's';
	}
}

char Vision::find_line()
{
	mutex->lock();
	cv::Mat mask = image.clone();
	mutex->unlock();
	
	//image is empty
	if(mask.empty()){
		return 'E';
	}
	
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Point> maxContour;
	
	//Detect black line:
	cv::cvtColor(mask, mask, cv::COLOR_BGR2HSV);
	cv::inRange(mask, cv::Scalar(0, 0, 0, 0), cv::Scalar(180, 255, 70, 0), mask);

	cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	//If contour(s) found, analyse img
	if (contours.size() != 0) {
		std::vector<int> indices(contours.size());
		std::iota(indices.begin(), indices.end(), 0); //fill indices from 0 to size of indices;

		//Sort indices of contours by size contours
		sort(indices.begin(), indices.end(), [&contours](int lhs, int rhs) {
			return cv::contourArea(contours[lhs]) > cv::contourArea(contours[rhs]);
		});

		//Biggest contour
		maxContour = contours[indices[0]];
		
		if (cv::contourArea(maxContour) > 1000){
			//Get extreme top and bottom point of line
			cv::Point extTop = *std::max_element(maxContour.begin(), maxContour.end(),
				[](const cv::Point& lhs, const cv::Point& rhs) {
				return lhs.y > rhs.y;
			});
			cv::Point extBot = *std::min_element(maxContour.begin(), maxContour.end(),
				[](const cv::Point& lhs, const cv::Point& rhs) {
				return lhs.y > rhs.y;
			});

			//Give instruction to follow line
			if ((extTop.x > image.rows / 3 * 1 && extTop.x < image.rows / 3 * 2 && extBot.x > image.rows / 3 * 1 && extBot.x < image.rows / 3 * 2) || extBot.y < image.rows / 4 * 3) {
				std::cout << "Go straight ahead" << std::endl;
				return 'F';
			}
			else if (extTop.x < image.rows / 3 * 1) {
				std::cout << "Go Left" << std::endl;
				return 'L';
			}
			else if (extTop.x > image.rows / 3 * 2) {
				std::cout << "Go Right" << std::endl;
				return 'R';
			}
			else {
				std::cout << "No instruction - follow last instruction" << std::endl;
				return 'I';
			}		
		}
	}
	
	//line not found
	return 'N';
}

bool Vision::find_waitPoint()
{
	mutex->lock();
	cv::Mat mask = image.clone();
	mutex->unlock();
	
	if(mask.empty()){
		return false;
	}
	
	cv::Mat lowerRed;
	cv::Mat upperRed;
	std::vector<std::vector<cv::Point>> contours;

	//Detect red(circle):
	cv::cvtColor(mask, mask, cv::COLOR_BGR2HSV);
	cv::inRange(mask, cv::Scalar(0, 70, 50), cv::Scalar(10, 225, 255), lowerRed);
	cv::inRange(mask, cv::Scalar(170, 70, 50), cv::Scalar(180, 225, 255), upperRed);
	cv::addWeighted(lowerRed, 1.0, upperRed, 1.0, 0.0, mask);

	cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	if (contours.size() != 0) {
		for (size_t i = 0; i < contours.size(); i++) {
			if (cv::contourArea(contours[i]) > 10000) {
				std::cout << "Found red circle" << std::endl;
				//If red(cirle) found, move forward and wait 30 seconds
				//std::cout << "Move forward a little bit more\nWait 30 seconds" << std::endl;
				//std::this_thread::sleep_for(std::chrono::seconds(30));
				//std::cout << "Move forward until line found and start following line again" << std::endl;
				return true;
			}
		}
	}

	return false;
}

Vision::~Vision() {
	delete mutex;
}