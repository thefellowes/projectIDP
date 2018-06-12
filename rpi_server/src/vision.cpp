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
		std::vector<std::string> names = { "low r", "low g", "low b", "upp R", "upp G", "upp B" };
		for (int i = 0; i < 7; i++)
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
		colorNames.push_back("black");
		colorNames.push_back("white");
	}

	int Vision::startVision(int number)
	{
		isActive = true;

		cv::VideoCapture cap(0);
		if (!cap.isOpened())
			return -1;
		cv::Mat frame;
		cap.grab();
		int count = 0;
		cap.retrieve(frame);

		while(isActive)
		{
			cap >> frame;
			image = frame;
			switch (number) {
				case 1:
					find_marker_cup();
					break;
				default:
					update(frame);
					break;
			}

			cv::imshow("image", frame);
			cv::waitKey(1);
		}

	}

	void Vision::stopVision() {
		isActive = false;
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
		int i = 0;
		while (i < lowerArrays.size())
		{
			color_pool.push_back(std::thread(&Vision::find_marker_by_color, this, std::ref(i)));
			i++;
		}

		i = 0;
		while (i < lowerArrays.size())
		{
			color_pool[i].join();
			i++;
		}

	}

	void Vision::find_marker_by_color(int i)
	{
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Point> contours1 = { { 0,0 } };
		std::vector<cv::Vec4i> hierarchy;
		int minArea = 3000;
		int maxArea = 30000;
		cv::Mat gray, edged, hsv_img, frame_threshed, thresh;
		cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
		cv::GaussianBlur(gray, gray, { 5, 5 }, 0);
		//cv::Canny(gray, edged, 35, 125);
		cv::cvtColor(image, hsv_img, cv::COLOR_BGR2HSV);
		cv::inRange(hsv_img, lowerArrays[i], upperArrays[i], frame_threshed);
		double ret = cv::threshold(frame_threshed, thresh, 127, 255, 0);
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
			cv::putText(image, colorNames[i], { middleX, middleY }, cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255, 255));
			cv::circle(image, { middleX, middleY }, (w + h) * 0.05, (0, 0, 255), -1);
		}

		minArea = 3000;
		markers[i] = cv::minAreaRect(contours1);
	}

	void Vision::find_marker_circles()
	{
		cv::Mat gray;
		cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
		// Reduce the noise so we avoid false circle detection
		std::vector<cv::Vec3f> circles;

		// Apply the Hough Transform to find the circles
		cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1, 30, 200, 50, 0, 0);
		int maxArea = 500;

		// Draw the circles detected
		for (size_t i = 0; i < circles.size(); i++)
		{
			cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
			cv::circle(image, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);// circle center     
			cv::circle(image, center, radius, cv::Scalar(0, 0, 255), 3, 8, 0);// circle outline
		}

	}
	void Vision::find_marker_white()
	{
		int scale = 4;
		cv::Mat src_gray, eroded;
		cv::cvtColor(image, src_gray, cv::COLOR_BGR2GRAY);
		resize(src_gray, src_gray, cv::Size(src_gray.cols / scale, src_gray.rows / scale)); // optionally resize image to speed up the process

		cv::rectangle(src_gray, cv::Rect(5, 5, src_gray.cols - 10, src_gray.rows - 10), cv::Scalar(0), 4); // correction 1
		src_gray = src_gray >127;

		std::vector<std::vector<cv::Point>> contours;

		for (int i = 2; i < src_gray.cols / 2; i++)
		{
			cv::Mat kernel = cv::Mat::ones(i, i, CV_8U);
			cv::erode(src_gray, eroded, kernel);

			findContours(eroded, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
			if (contours.size() == 1 & contours[0].size() < 5)
			{
				cv::resize(kernel, kernel, cv::Size(), 0.9, 0.9); // correction 2
				cv::dilate(eroded, eroded, kernel);
				cv::findContours(eroded, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
				cv::polylines(image, cv::Mat(contours[0]) * scale, true, cv::Scalar(0, 0, 255)); // resize up the contour and draw
				break;
			}
		}
	}

	void Vision::find_marker_cup()
	{
		find_marker_by_color(1);
		//find_marker_circles();
		// Show your results
		//cv::namedWindow("Hough Circle Transform Demo", cv::WINDOW_AUTOSIZE);
		//cv::imshow("Hough Circle Transform Demo", image);
	}
	
	void Vision::find_line()
	{
		cv::Mat mask = image.clone();
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Point> maxContour;
		
		//Detect dark line:
		cv::cvtColor(mask, mask, cv::COLOR_RGB2GRAY);
		cv::GaussianBlur(mask, mask, cv::Size(5, 5), 0, 0);
		cv::threshold(mask, mask, 60, 255, cv::THRESH_BINARY_INV);

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
			}
			else if (extTop.x < image.rows / 3 * 1) {
				std::cout << "Go Left" << std::endl;
			}
			else if (extTop.x > image.rows / 3 * 2) {
				std::cout << "Go Right" << std::endl;
			}
			else {
				std::cout << "No instruction" << std::endl;
			}
		}
	}

	void Vision::find_waitPoint()
	{
		cv::Mat mask = image.clone();
		cv::Mat lowerRed;
		cv::Mat upperRed;
		std::vector<std::vector<cv::Point>> contours;
		
		//Detect red(circle):
		cv::cvtColor(mask, mask, cv::COLOR_BGR2HSV);
		cv::inRange(mask, cv::Scalar(0, 70, 50), cv::Scalar(10, 225, 255), lowerRed);
		cv::inRange(mask, cv::Scalar(170, 70, 50), cv::Scalar(180, 225, 255), upperRed);
		cv::addWeighted(lowerRed, 1.0, upperRed, 1.0, 0.0, mask);

		cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		//Only exucute if haven't wait before
		//if (!passedWaitPoint) {
			if (contours.size() != 0) {
				for (int i = 0; i < contours.size(); i++) {
					if (cv::contourArea(contours[i]) > 10000) {
						//If red(cirle) found, move forward and wait 30 seconds
						std::cout << "Move forward a little bit more\nWait 30 seconds" << std::endl;
						//std::this_thread::sleep_for(std::chrono::seconds(30));
						//passedWaitPoint = true;
					}
				}
			}
		//}
	}
