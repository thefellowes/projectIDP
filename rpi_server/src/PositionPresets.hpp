#ifndef PositionPresets_HPP
#define PositionPresets_HPP

#include "stdafx.h"

#include <vector>
#include <string>

class PositionPresets {
public:

	PositionPresets() {
		this->IDs = {
			"Container_1",
			"Container_2"
		};

		this->presets = { {
			{ {
				{ 0.0f, 0.0f, 0.0f, 0.0f }
			} },
			{ {
				{ 0.0f, 0.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f, 0.0f }
			} }
		} };

		if (IDs.size() > presets.size()) std::cout << "ERROR: Preset ID list contains more items than Positions list" << std::endl;
		else if (IDs.size() < presets.size()) std::cout << "ERROR: Positions list contains more items than Preset ID list" << std::endl;
		else this->size = IDs.size();

	}

	std::vector<std::vector<float>> get(std::string p) {
		for (size_t i = 0; i < size; i++)
			if (p == IDs[i]) return presets[i];
	}

	//std::vector<std::vector<float>> get(std::string p)
	//{
	//	if (p== "container_1") {
	//		return { { { 3.0f, 2.0f, 10.0f, 50.0f } } };
	//	}
	//	else if (p == "container_2") {
	//		return { { { 3.0f, 2.0f, 10.0f, 50.0f },
	//		{ 1.5f, 5.5f, 10.2f, 2938.1034f } } };
	//	}

	//}

private:
	int size;
	std::vector<std::string> IDs;
	std::vector<std::vector<std::vector<float>>> presets;
};

#endif