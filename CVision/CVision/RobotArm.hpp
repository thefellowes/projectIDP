#ifndef RobotArm_HPP
#define RobotArm_HPP

#include "stdafx.h"

#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

class RobotArm {
public:

	RobotArm(float lengthArm_1, float lengthArm_2) {
		this->length1 = lengthArm_1;
		this->length2 = lengthArm_2;
	}

	std::vector<std::vector<int>> getPath(float x1, float y1, float x2, float y2, float ha1, float ha2, float angleOffset = 5.0f) {
		// Initiate 3rd point for bezier, along with the pathlength
		float bx = 0;
		float by = 0;
		float pathLength = 0;

		// Difference x, difference y, difference head angle
		float dx = x2 - x1;
		float dy = y2 - y1;
		float dha = ha2 - ha1;

		// If the angle of the movement is horizontal or vertical enough (within the offset), make a linear path. 
		// Otherwise use a bezier curve. 
		float a = atan2(dy, dx) * 180 / M_PI;
		float ao = angleOffset;
		if (inBounds(a, 0.0f + ao, 90.0f - ao) || inBounds(a, -90.0f-ao, -180.0f+ao)) {
			// On a / angled line, take the top-left point as 3rd point. 
			bx = x1 < x2 ? x1 : x2;
			by = y1 > y2 ? y1 : y2;
			pathLength = getBezierPathLength(x1, y1, x2, y2, bx, by);
		}
		else if (inBounds(a, 90.0f + ao, 180.0f - ao) || inBounds(a, 0.0f - ao, -90.0f + ao)) {
			// On a \ angled line, take the top-right point as 3rd point.
			bx = x1 > x2 ? x1 : x2;
			by = y1 > y2 ? y1 : y2;
			pathLength = getBezierPathLength(x1, y1, x2, y2, bx, by);
		}
		else {
			//On a linear path
			bx = (x1 + x2) / 2;
			by = (y1 + y2) / 2;
			pathLength = sqrt(pow(dx, 2) + pow(dy, 2));
		}

		int positionErrorCount = 0;
		bool positionPossible = true;
		std::vector<std::vector<int>> path;
		for (float i = 0; i <= pathLength; i += 0.1)
		{
			// Beginning and end vector of virtual line
			float xa = getPt(x1, bx, i);
			float ya = getPt(y1, by, i);
			float xb = getPt(bx, x2, i);
			float yb = getPt(by, y2, i);

			// Vector of final position on the virtual line
			float x = getPt(xa, xb, i);
			float y = getPt(ya, yb, i);

			float ha = ha1 + (dha * i);

			std::vector<int> servoValues = PosToAngles(x, y, ha);
			positionPossible = constraint(servoValues, constraints_min, constraints_max);
			if (!positionPossible) positionErrorCount++;

			path.push_back(servoValues);
		}
		if (positionErrorCount > 0) {
			std::cout << "WARNING: " << positionErrorCount << "/" << pathLength * 10 << " positions can't be reached" << std::endl;
		}
		if (!positionPossible) {
			std::cout << "WARNING: The end position can't be reached" << std::endl;
		}

		return path;
	}

	std::vector<int> PosToAngles(float x, float y, char headAngle=90.0) {

		float c2 = (pow(x, 2) + pow(y, 2) - pow(length1, 2) - pow(length2, 2)) / (2 * length1 * length2);
		float s2 = sqrt(1 - pow(c2, 2));

		float K1 = length1 + length2 * c2;
		float K2 = length2 * s2;

		float theta = atan2(y, x) - atan2(K2, K1);
		float psi = atan2(s2, c2);

		float angle1 = theta * 180 / M_PI;
		float angle2 = psi * 180 / M_PI;
		float angle3 = angle1 + angle2 + headAngle;

		int servoValue1 = roundf((angle1 * 1023.0f) / 300) + defaultValues[0];
		int servoValue2 = roundf((angle2 * 1023.0f) / 300) + defaultValues[1];
		int servoValue3 = roundf((angle3 * 1023.0f) / 300) + defaultValues[2];

		servoValues = { servoValue1, servoValue2, servoValue3 };

		return servoValues;
	}

	bool constraint(std::vector<int> &values, std::vector<int> constraints_min, std::vector<int> constraints_max)
	{
		bool result = true;
		if (values.size() == constraints_min.size() && constraints_min.size() == constraints_max.size()) {
			int i = 0;
			for (int v : values) {
				if (v < constraints_min[i]) {
					values[i] = constraints_min[i];
					result = false;
				}
				else if (v > constraints_max[i]) {
					values[i] = constraints_min[i];
					result = false;
				}
				i++;
			}
		}
		else {
			std::cout << "WARNING: Input Vector sizes aren't the same. No constraints were done" << std::endl;
			result = false;
		}
		return result;
	}

	//std::vector<int> anglesToServoValues(std::vector<float> angles)
	//{
	//	std::vector<int> result;
	//	int c = 0;
	//	for (float a : angles) {
	//		// angle of 0 results into a value of 0. Angle of 300 results into value of 1023
	//		result.push_back(roundf((a * 1023.0f) / 300) + defaultValues[c]);
	//		c++;
	//	}
	//}

	

	//std::vector<std::vector<float>> PosToAngles(const std::vector<std::vector<float>> &path) {
	//	std::vector<std::vector<float>> result;
	//	for (auto pos : path) {
	//		result.push_back(PosToAngles(pos[0], pos[1], pos[2]));
	//	}
	//	return result;
	//}

	//std::vector<std::vector<float>> getLinearPath(float x1, float y1, float x2, float y2, float headAngle1, float headAngle2) {
	//	float dX = x2 - x1;
	//	float dY = y2 - y1;
	//	float lenC = sqrt(pow(dX, 2) + pow(dY, 2));
	//	float dHeadAngle = headAngle2 - headAngle1;
	//	
	//	std::vector<std::vector<float>> path;
	//	for (float i = 0; i < lenC; i+=0.1)
	//	{
	//		float percent = i / lenC;
	//		float xNew = dX * percent + x1;
	//		float yNew = dY * percent + y1;
	//		float headAngleNew = dHeadAngle * percent + headAngle1;

	//		path.push_back(PosToAngles(xNew, yNew, headAngleNew));

	//	}

	//	return path;
	//	
	//}

	

	

	//std::vector<int> angleToServoValue(std::vector<float> angles)
	//{
	//	std::vector<int> result;
	//	int c = 0;
	//	for (float a : angles) {
	//		//if (!inBounds(a, 0.0f, 300.0f)) {
	//		//	printf("WARNING: Angle Out Of Bounds - Minimal value: 0.0f, Maximal value: 300.0f");
	//		//	if (a > 300.0f) a = 300.0f;
	//		//	else if (a < 0.0f) a = 0.0f;
	//		//}
	//			// angle of 0 results into a value of 0. Angle of 300 results into value of 1023
	//		result.push_back(roundf((a * 1023.0f) / 300) + defaultValues[c]);
	//		c++;
	//	}
	//}

	////Result of '2' means that the end position isn't within reach due to constraints.
	////Result of '1' means that the end position can be reached, but other positions in the path can't be reached.
	//int angleToServoValue(std::vector<std::vector<float>> &path, std::vector<int> constraints_min, std::vector<int> constraints_max) {
	//	bool pathPossible = true;
	//	bool endPositionPossible = true;
	//	for (auto pos : path) 
	//	{
	//		std::vector<int> values = angleToServoValue(pos);
	//		endPositionPossible = constraint(values, constraints_min, constraints_max, false);
	//		if (pathPossible && !endPositionPossible)
	//			pathPossible = false;
	//	}
	//	if (!endPositionPossible) {
	//		std::cout << "WARNING: The end position can't be reached" << std::endl;
	//		return 2;
	//	}
	//	else if (!pathPossible) {
	//		std::cout << "WARNING: Some positions in this path can't be reached" << std::endl;
	//		return 1;
	//	}
	//	return 0;
	//}

	//bool constraint(std::vector<int> &values, std::vector<int> constraints_min, std::vector<int> constraints_max, bool showError=true)
	//{
	//	bool result = true;
	//	if (values.size() == constraints_min.size() && constraints_min.size() == constraints_max.size()) {
	//		int i = 0;
	//		for (int v : values) {
	//			if (v < constraints_min[i]) {
	//				if (showError) std::cout << "WARNING: Minimal constraint on Servo " << i << " exceeded." << std::endl;
	//				values[i] = constraints_min[i];
	//				result = false;
	//			}
	//			else if (v > constraints_max[i]) {
	//				if (showError) std::cout << "WARNING: Maximal constraint on Servo " << i << " exceeded." << std::endl;
	//				values[i] = constraints_min[i];
	//				result = false;
	//			}
	//			i++;
	//		}
	//	}
	//	else {
	//		std::cout << "WARNING: Input Vector sizes aren't the same. No constraints were done" << std::endl;
	//		result = false;
	//	}
	//	return result;
	//}

	/*std::vector<std::vector<float>> getBezierPath(float x1, float y1, float x2, float y2, float ha1, float ha2) {
		std::vector<std::vector<float>> path = { {} };

		//If ha2 == ha1 -> return getLinear path
		if (ha1 == ha2) return getLinearPath(x1, y1, x2, y2, ha1, ha2);
		
		float dx = x2 - x1;
		float dy = y2 - y1;
		float c = sqrt(pow(dx,2) + pow(dy,2));
		// angle of (dx,dy)
		float a = atan2(dy, dx);
		// difference head angle
		float dha = (ha2-ha1) * M_PI/180;
		// angle of point 3
		//float ap3 = a - dha;
		// -rotate the normal of point 3 and 
		// -give it the length of half of the linear translation and
		// -translate it to the right position
		float bx = cos(a-dha) * c/2 + (x1+x2)/2;
		float by = sin(a-dha) * c/2 + (y1+y2)/2;
		

		for (float i = 0; i <= 1; i += 0.1)
		{
			// The Green Line
			float xa = getPt(x1, bx, i);
			float ya = getPt(y1, by, i);
			float xb = getPt(bx, x2, i);
			float yb = getPt(by, y2, i);

			// The Black Dot
			float x = getPt(xa, xb, i);
			float y = getPt(ya, yb, i);
			// 5.0 - 5.95 - 6.8 - 7.5 - 8.2 - 8.75 - 9.2 - 9.5 - 9.8 - 9.95 - 10
			if (true) {
				int x = 0;
			}
		}

		return path;
	}*/

private:
	float length1;
	float length2;
	std::vector<int> servoValues;
	std::vector<int> defaultValues = { 0,0,0 };
	std::vector<int> constraints_min = { 0,0,0 };
	std::vector<int> constraints_max = { 1023,1023,1023 };

	float getPt(float n1, float n2, float perc)
	{
		float diff = n2-n1;
		return n1 + (diff * perc);
	}

	bool inBounds(float value, float low, float high) {
		if (value > low && value < high) return true;
		return false;
	}

	float getBezierPathLength(float x1, float y1, float x2, float y2, float bx, float by) {
		float xOld = x1;
		float yOld = y1;
		float length = 0;
		for (float i = 0; i <= 1; i += 0.01)
		{
			// Beginning and end vector of virtual line
			float xa = getPt(x1, bx, i);
			float ya = getPt(y1, by, i);
			float xb = getPt(bx, x2, i);
			float yb = getPt(by, y2, i);

			// Vector of final position on the virtual line
			float x = getPt(xa, xb, i);
			float y = getPt(ya, yb, i);

			float c = sqrt(pow(x - xOld, 2) + pow(y - yOld, 2));
			length += c;

			xOld = x;
			yOld = y;
		}
		return length;
	}

public:

	std::vector<int> getServoValues() { return servoValues; }


};

#endif // RobotArm.hpp