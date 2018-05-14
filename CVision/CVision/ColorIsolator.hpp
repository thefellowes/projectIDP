#ifndef ColorIsolator_HPP
#define ColorIsolator_HPP

#include "stdafx.h"
#include "opencv2\opencv.hpp"
#include "Sliders.hpp"

#include <vector>
#include <string>

class ColorIsolator {
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

	ColorIsolator(bool showControls=true, std::vector<int> initValues={}, std::string windowName="", std::vector<std::string> sliderNames = {}) {
		std::vector<std::string> defaultNames = { "low r", "low g", "low b", "upp R", "upp G", "upp B" };
		std::vector<int> defaultInitValues = { 0,0,0,255,255,255 };

		this->windowName = windowName == "" ? id_generator(100000, 999999) : windowName;
		appendWhereNone(sliderNames, defaultNames);
		appendWhereNone(initValues, defaultInitValues);

		this->sliders = Sliders(windowName, defaultNames.size(), "", 0, 255, true, sliderNames, {}, {}, initValues);
	}

	Mat getMask(Mat &frame) {
		Mat hsv;
		cvtColor(frame, hsv, COLOR_BGR2HSV);
		std::vector<int> lower = { sliders.val(0), sliders.val(1), sliders.val(2) };
		std::vector<int> upper = { sliders.val(3), sliders.val(4), sliders.val(5) };
		Mat mask;
		inRange(frame, lower, upper, mask);
		return mask;
	}

private:
	std::string windowName;
	Sliders sliders;

public:

	

};

#endif // ColorIsolator_HPP