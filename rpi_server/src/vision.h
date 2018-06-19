
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
	int programNumber;

public:
	bool doUpdateFrame;

	Vision(std::vector<std::vector<int>> initValues);

	cv::Mat getImage();

	void update(cv::Mat image_);

	int startVision();

	void stopVision();

	void setProgram(int number);

	void updateMarkers();

	std::string getStance(int h, int w);

	void find_markers();

	bool find_marker_by_color(int i);

	bool find_marker_cup();

	char find_line();

	bool find_waitPoint();

};

#endif

