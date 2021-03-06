
#ifndef VISION_H
#define VISION_H
#include "opencv2/opencv.hpp"
#include <vector>
#include <string>

class Vision {

private:
	bool isActive;

	//int middleX, middleY, x, y, w, h = 0;
	std::vector<cv::RotatedRect> markers;
	std::vector<std::vector<int>> lowerArrays;
	std::vector<std::vector<int>> upperArrays;
	std::vector<std::string> colorNames;
	cv::Mat image;

public:

	Vision(std::vector<std::vector<int>> initValues);

	cv::Mat getImage();

	void update(cv::Mat image_);

	int startVision();
	void stopVision();

	void updateMarkers();

	std::string getStance(int h, int w);

	void find_markers(cv::Mat image, std::vector<std::vector<int>> lowerArrays, std::vector<std::vector<int>> upperArrays);

	void find_marker_by_color(cv::Mat image, std::vector<std::vector<int>> lowerArrays, std::vector<std::vector<int>> upperArrays, int i);

	void find_marker_cup(cv::Mat image);

};

#endif

