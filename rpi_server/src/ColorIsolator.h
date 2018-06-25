#ifndef ColorIsolator_HPP
#define ColorIsolator_HPP

#include "opencv2/opencv.hpp"
#include <vector>
#include <string>

class ColorIsolator {

	bool showControls;
	std::vector<int> initValues;
	std::string windowName;
	std::vector<std::string> sliderNames;
	std::vector<int> lower = { 0, 0, 0 };
	std::vector<int> upper = { 255, 255, 255 };
	std::vector<int> minVal = { 0, 0, 0 };
	std::vector<int> maxVal = { 255, 255, 255 };

public:
	ColorIsolator() : initValues() {};
	ColorIsolator(std::vector<int> initValues1)
	{
		initValues = initValues1;
		lower = { initValues[0], initValues[1], initValues[2] };
		upper = { initValues[3], initValues[4], initValues[5] };
	}

	cv::Mat isolate(cv::Mat frame) {
		cv::cvtColor(frame, frame, cv::COLOR_BGR2HSV);
		cv::Mat newImage;
		cv::inRange(frame, lower, upper, newImage);
		return newImage;
	}

	std::vector<int> getLower()
	{
		return lower;
	}

	std::vector<int> getUpper()
	{
		return upper;
	}

	std::string id_generator(std::vector<char> chars)
	{
		int randNum = rand() % (chars.size()- 1);
		std::string s;
		s.push_back(chars[randNum]);
		return s;
	}

	std::vector<std::string> appendWhereNoneString(std::vector<std::string> origin, std::vector<std::string> toAdd)
	{
		std::vector<std::string> returnOrigin = origin;
		int size = toAdd.size();
		for (int i = 0; i < size; i++)
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
		int size = toAdd.size();
		for (int i = 0; i < size; i++)
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