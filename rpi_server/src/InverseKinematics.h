#define _USE_MATH_DEFINES


#include <vector>
#include <math.h>
#include <iostream>
#include "armConstantes.h"


//Grab these from config file in a more finalized project
//int length1 = 5;
//int length2 = 5;
//std::vector<int> defaultValues = { 0,0,150 };
//std::vector<int> constraints_min = { 0,0,0 };
//std::vector<int> constraints_max = { 1023,1023,1023 };

std::vector<int> posToAnglesNoOOR(float x, float y, float headAngle=270.0f);
std::vector<int> posToAngles(float x, float y, float headAngle=270.0f);
int angleToServoValue(float angle, int defaultValue);

std::vector<std::vector<int>> getPath(float x1, float y1, float x2, float y2, float ha1, float ha2, float angleOffset = 5.0f);
bool constraint(std::vector<int> &values, std::vector<int> constraints_min, std::vector<int> constraints_max);
float getBezierPathLength(float x1, float y1, float x2, float y2, float bx, float by);
float getPt(float n1, float n2, float perc);
bool inBounds(float value, float low, float high);

template <typename T>
T convertToRange(const T value, const T originalMax, const T newMax);
template <typename T>
T convertToRange(const T value, const T originalMin, const T originalMax, const T newMin, const T newMax);