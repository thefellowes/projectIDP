
#ifndef functions_h
#define functions_h
#include "stdafx.h"
#include "opencv2\opencv.hpp"
#include <vector>
#include <string>
#include "ColorIsolator.hpp"
#include "Sliders.hpp"
using namespace cv;

class functions {

private:
	int middleX, middleY, x, y, w, h = 0;
	RotatedRect marker;
	ColorIsolator isolator;
	Sliders sliders;
	Mat image;

	void setCoordinates(int x, int y, int w, int h, int middleX, int middle_Y);

public:

	functions(std::vector<int> initValues);

	Mat getImage();

	void update(Mat image_);

	std::string getStance();

	RotatedRect find_marker(Mat image, std::vector<int> lowerArray, std::vector<int> upperArray);

	void find_marker_cup(Mat image);

};

#endif

