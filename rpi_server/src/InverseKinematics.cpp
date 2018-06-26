#include "InverseKinematics.h"



// Position to Angles No Out Of Range
// When out of range, this will return the angles of the position closest to the required position
std::vector<int> posToAnglesNoOOR(float x, float y, float headAngle) {
	if (x*x + y*y > (l1+l2)*(l1+l2)) {
		float vectorSize = sqrt(x*x + y * y);
		x = (x / vectorSize) * ((l1+l2)*0.9999999);
		y = (y / vectorSize) * ((l1+l2)*0.9999999);
	}
	return posToAngles(x, y, headAngle);
}

// Returns { -1, -1, -1 } when position is not reachable
std::vector<int> posToAngles(float x, float y, float headAngle) {

	if (x*x + y*y > (l1+l2)*(l1+l2)) {
		return { -1, -1, -1 };
	}

	float c2 = (pow(x, 2) + pow(y, 2) - pow(l1, 2) - pow(l2, 2)) / (2 * l1 * l2);
	float s2 = sqrt(1 - pow(c2, 2));

	float K1 = l1 + l2 * c2;
	float K2 = l2 * s2;

	float theta = atan2(y, x) - atan2(K2, K1);
	float psi = atan2(s2, c2);

	float angle1 = theta * 180 / M_PI;
	float angle2 = psi * 180 / M_PI;
	float angle3 = headAngle - (angle1 + angle2);

	if (angle1 < 0) angle1 += 360;
	if (angle3 < -180) angle3 += 360;
	if (angle3 > 180) angle3 -= 360;

	int servoValue1 = angleToServoValue(angle1, defaultValues[0]);
	int servoValue2 = angleToServoValue(angle2, defaultValues[1]);
	int servoValue3 = angleToServoValue(angle3, defaultValues[2]);

	return { servoValue1, servoValue1, servoValue2, servoValue3 };
}

int angleToServoValue(float angle, int defaultValue) {
	return roundf(convertToRange(angle, 300.0f, 1023.0f)) + defaultValue;
}

void anglesToPos(std::vector<int> angles, float &x, float &y) {
	int servoValue1 = (angles[0] + angles[1]) / 2;
	int servoValue2 = angles[2];

	int angle1 = convertToRange((float)(servoValue1 - defaultValues[0]), 1023.0f, 300.0f);
	int angle2 = convertToRange((float)(servoValue2 - defaultValues[1]), 1023.0f, 300.0f);

	float xPt1 = cos(angle1 * M_PI / 180)*l1;
	float yPt1 = sin(angle1 * M_PI / 180)*l1;

	float xPt2 = cos((angle2 + angle1)*M_PI / 180)*l2 + xPt1;
	float yPt2 = sin((angle2 + angle1)*M_PI / 180)*l2 + yPt1;

	x = xPt2;
	y = yPt2;
}

std::vector<std::vector<int>> getPath(float x1, float y1, float x2, float y2, float ha1, float ha2, float angleOffset) {
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
	if (inBounds(a, 0.0f + ao, 90.0f - ao) || inBounds(a, -180.0f + ao, -90.0f - ao)) {
		// On a / angled line, take the top-left point as 3rd point. 
		bx = x1 < x2 ? x1 : x2;
		by = y1 > y2 ? y1 : y2;
		pathLength = getBezierPathLength(x1, y1, x2, y2, bx, by);
	}
	else if (inBounds(a, 90.0f + ao, 180.0f - ao) || inBounds(a, -90.0f + ao, 0.0f - ao)) {
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
	float i = 0;
	while (i < pathLength)
	{
		i += moveToStepSize;
		if (i > pathLength) i = pathLength;
		float perc = i / pathLength;

		// Beginning and end vector of virtual line
		float xa = getPt(x1, bx, perc);
		float ya = getPt(y1, by, perc);
		float xb = getPt(bx, x2, perc);
		float yb = getPt(by, y2, perc);

		// Vector of final position on the virtual line
		float x = getPt(xa, xb, perc);
		float y = getPt(ya, yb, perc);
		// the angle of the head
		float ha = ha1 + (dha * perc);

		std::vector<int> servoValues = posToAnglesNoOOR(x, y, ha);
		positionPossible = constraint(servoValues, constr_min.joints, constr_max.joints);
		if (!positionPossible) positionErrorCount++;

		path.push_back(servoValues);

		i++;
	}
	if (positionErrorCount > 0) {
		std::cout << "WARNING: " << positionErrorCount << "/" << path.size() << " positions can't be reached" << std::endl;
	}
	if (!positionPossible) {
		std::cout << "WARNING: The end position can't be reached" << std::endl;
	}

	return path;
}

//The values vector will be
bool constraint(std::vector<int> &values, std::vector<int> constr_min, std::vector<int> constr_max)
{
	bool result = true;
	if (values.size() == constr_min.size() && constr_min.size() == constr_max.size()) {
		int i = 0;
		for (int v : values) {
			if (v < constr_min[i]) {
				values[i] = constr_min[i];
				result = false;
			}
			else if (v > constr_max[i]) {
				values[i] = constr_min[i];
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

// Calculate the length of a bezier curve by calculating the distance between 50 points on the curve.
float getBezierPathLength(float x1, float y1, float x2, float y2, float bx, float by) {
	float xOld = x1;
	float yOld = y1;
	float length = 0;
	for (float i = 0; i <= 1; i += 0.02)
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

float getPt(float n1, float n2, float perc)
{
	float diff = n2 - n1;
	return n1 + (diff * perc);
}

bool inBounds(float value, float low, float high) {
	if (value > low && value < high) return true;
	return false;
}

template <typename T>
T convertToRange(const T value, const T originalMax, const T newMax) {
	return (value * newMax) / originalMax;
}
template <typename T>
T convertToRange(const T value, const T originalMin, const T originalMax, const T newMin, const T newMax) {
	//return newMin + ((value * (newMax - newMin)) / (originalMax - originalMin));
	return newMin + ((value-originalMin) / (originalMax-originalMin) * (newMax - newMin));
}