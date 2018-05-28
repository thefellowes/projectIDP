#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#ifndef Sliders_HPP
#define Sliders_HPP

using namespace cv;

class Sliders {
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

	Sliders(std::string windowName = "", int amount = 6, std::string defaultSliderName = "", int defaultMinValue = 0, int defaultMaxValue = 100, bool newWindow = true, std::vector<std::string> sliderNames = {}, std::vector<int> minValues = {}, std::vector<int> maxValues = {}, std::vector<int> initValues = {}) {
		if (windowName == "") {
			windowName = id_generator(10000000, 99999999);
			newWindow = true;
		}
		if (newWindow) {
			namedWindow(windowName, 0);
			
		}
		
		this->windowName = windowName;
		this->sliderNames = sliderNames;
		this->amount = amount;

		defaultSliderName = defaultSliderName == "" ? id_generator(100000, 999999) : defaultSliderName;
		for (int i = 0; i < amount; i++)
		{
			if (i == sliderNames.size()) {
				sliderNames.push_back(defaultSliderName + " (" + std::to_string(i) + ")");
			}

			int minVal = i < minValues.size() ? minValues[i] : defaultMinValue;
			int maxVal = i < maxValues.size() ? maxValues[i] : defaultMaxValue;
			int initVal = i < initValues.size() ? initValues[i] : minVal;

			createTrackbar(sliderNames[i], windowName, &minVal, maxVal);
			setTrackbarPos(sliderNames[i], windowName, initVal);
		}

	}

	int val(int i) {
		if (i < amount && i >= 0) {
			return getTrackbarPos(sliderNames[i], windowName);
		}
		else {
			return -1;
		}
	}



private:
	std::string windowName;
	std::vector<std::string> sliderNames;
	int amount;

public:



};
#endif // ColorIsolator_HPP
