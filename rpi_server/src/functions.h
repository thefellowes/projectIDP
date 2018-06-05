
#ifndef functions_h
#define functions_h
#include "opencv2/opencv.hpp"
#include <vector>
#include <string>
#include "ColorIsolator.h"

class functions {

private:
	int middleX, middleY, x, y, w, h = 0;
	std::vector<cv::RotatedRect> markers;
	std::vector<ColorIsolator> isolators;
	std::vector<std::string> colorNames;
	cv::Mat image;

	void setCoordinates(int x, int y, int w, int h, int middleX, int middle_Y);

public:

	functions(std::vector<std::vector<int>> initValues);

	cv::Mat getImage();

	void update(cv::Mat image_);

	void updateMarkers();

	std::string getStance();

	cv::RotatedRect find_marker(cv::Mat image, std::vector<int> lowerArray, std::vector<int> upperArray, std::string colorName);

	void find_marker_cup(cv::Mat image);

};

#endif

