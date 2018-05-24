#ifndef ColorIsolator_HPP
#define ColorIsolator_HPP

#include "stdafx.h"
#include "opencv2\opencv.hpp"
#include "Sliders.hpp"
#include <vector>
#include <string>

class ColorIsolator {

	bool showControls;
	std::vector<int> initValues;
	std::string windowName;
	std::vector<std::string> sliderNames;
	Sliders sliders;
	std::vector<int> minVal = { 0, 0, 0 };
	std::vector<int> maxVal = { 255, 255, 255 };

public:
	ColorIsolator() : showControls(true), initValues(), windowName(""), sliderNames(), sliders(), minVal(), maxVal() {};
	ColorIsolator(std::vector<int> initValues1, std::string windowName1, std::vector<std::string> sliderNames1)
	{
		sliderNames = sliderNames1;
		initValues = initValues1;
		windowName = windowName1;
		showControls = true;
		std::vector<std::string> names = { "low r", "low g", "low b", "upp R", "upp G", "upp B" };
		std::vector<int> defaultInitValues = { 0, 0, 0, 255, 255, 255 };

		if (windowName == "")
		{
			char chars[26] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z' };
			windowName = id_generator(chars);
		}

		sliderNames = appendWhereNoneString(sliderNames, names);
		initValues = appendWhereNoneInt(initValues, defaultInitValues);
		sliders = Sliders(windowName, names.size(), "", 0, 255, true, sliderNames, {}, {}, initValues);
	}

	Mat isolate(Mat frame) {
		cvtColor(frame, frame, COLOR_BGR2HSV);

		std::vector<int> lower = { sliders.val(0), sliders.val(1), sliders.val(2) };
		std::vector<int> upper = { sliders.val(3), sliders.val(4), sliders.val(5) };

		Mat newImage;
		inRange(frame, lower, upper, newImage);
		return newImage;
	}

	std::vector<int> getLower()
	{
		std::vector<int> toReturn;
		toReturn.push_back(sliders.val(0));
		toReturn.push_back(sliders.val(1));
		toReturn.push_back(sliders.val(2));
		return toReturn;
	}

	std::vector<int> getUpper()
	{
		std::vector<int> toReturn;
		toReturn.push_back(sliders.val(3));
		toReturn.push_back(sliders.val(4));
		toReturn.push_back(sliders.val(5));
		return toReturn;
	}

	std::string id_generator(char chars[])
	{
		int randNum = rand() % (sizeof(chars) - 1);
		std::string s;
		s.push_back(chars[randNum]);
		return s;
	}

	std::vector<std::string> appendWhereNoneString(std::vector<std::string> origin, std::vector<std::string> toAdd)
	{
		std::vector<std::string> returnOrigin = origin;
		for (int i = 0; i < toAdd.size(); i++)
		{
			if (returnOrigin.size() < toAdd.size())
			{
				returnOrigin.push_back(toAdd[i]);
			}
		}
		return returnOrigin;
	}

	std::vector<int> appendWhereNoneInt(std::vector<int> &origin, std::vector<int> &toAdd)
	{
		std::vector<int> returnOrigin = origin;
		for (int i = 0; i < toAdd.size(); i++)
		{
			if (returnOrigin.size() < toAdd.size())
			{
				returnOrigin.push_back(toAdd[i]);
			}
		}
		return returnOrigin;
	}
};

#endif // ColorIsolator_HPP