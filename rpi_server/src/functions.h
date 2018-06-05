
#ifndef functions_h
#define functions_h
#include "opencv2/opencv.hpp"
#include <vector>
#include <string>

class functions {

private:
	int middleX, middleY, x, y, w, h = 0;
	std::vector<cv::RotatedRect> markers;
	std::vector<std::vector<int>> lowerArrays;
	std::vector<std::vector<int>> upperArrays;
	std::vector<std::string> colorNames;
	cv::Mat image;

public:

	functions(std::vector<std::vector<int>> initValues);

	cv::Mat getImage();

	void update(cv::Mat image_);

	void updateMarkers();

	std::string getStance();

	std::vector<cv::RotatedRect> functions::find_markers(cv::Mat image, std::vector<std::vector<int>> lowerArrays, std::vector<std::vector<int>> upperArrays);

	void find_marker_cup(cv::Mat image);

};

#endif

