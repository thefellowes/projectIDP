#ifndef ARMCONSTANTES_H
#define ARMCONSTANTES_H

#include <vector>

struct ArmServos {
	int armRotation;
	std::vector<int> joints;
	int gripperRotation;
	int gripper;
};

//Speed of Gripper servo
const int gripperSpeed = 200;

//MIN and MAX turn value of servo
const int servoMinRotation = 0;
const int servoMaxRotation = 1023;

//MIN and MAX speed value of servo
const int servoMinSpeed = 0;
const int servoMaxSpeed = 1023;

//Length of arm parts
const int l1 = 13;
const int l2 = 13;
//Default values where the angles in the real world is 0 (arm laying flat backwards) 
const std::vector<int> defaultValues = { 210,512,512 };
//MIN and MAX constraint values of the servos
const ArmServos constr_min = { 0, { 131,30,187 }, 0, 600 };
const ArmServos constr_max = { 1023, { 902,991,830 }, 1023, 900 };

const float maxSpeed = 0.175f;	//size to change position
const float maxSpeedRotation = 10;	//size to change rotation

const int moveDelay = 10;
const float moveToStepSize = 0.1f;
const int moveToDelay = (moveToStepSize / maxSpeed * maxSpeedRotation);

#endif