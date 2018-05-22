#include <thread>
#include <chrono>
#include "InverseKinematics.h"
#include "arm.h"

//debug:
#include <iostream>

//MIN and MAX turn value of servo
const int servoMinRotation = 0;
const int servoMaxRotation = 1023;

int l1 = 13;
int l2 = 13;
std::vector<int> defaultVal = { 200,512,512 };
std::vector<int> constr_min = { 210,0,0 };
std::vector<int> constr_max = { 900,1023,1023 };

const float Arm::posDifference = 0.5;	//size to change position
const float Arm::rotDifference = 10;	//size to change rotation

bool Arm::posPossible(int x, int y)
{
	int armlength = l1 + l2;
	return (x * x + y * y > armlength * armlength) ? false : true;
}

Arm::Arm(AX12A &servoControl, std::vector<int> servoIDs)
{
	ax12a = servoControl;
	this->servoIDs = servoIDs;
	posX = 0;
	posY = l1 + l2;	//stands in inversekinematics.cpp
	posRotation = 512;
	headAngle = 90.0;

	//set servo's in default position
	ax12a.move(servoIDs[0], posRotation);
	//std::this_thread::sleep_for(std::chrono::milliseconds(20));

	std::vector<int> newPos = posToAngles(posX, posY, headAngle);
	bool newPosPossible = constraint(newPos, constr_min, constr_max);

	//extra check if position is possible
	if (newPosPossible) {
		for (int i = 0; i < newPos.size(); i++) {
			ax12a.moveSpeed(servoIDs[i + 1], newPos[i], 300);
			//std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}

	std::this_thread::sleep_for(std::chrono::seconds(1));
}

// speedX and speedY variable between -1.0 and 1.0
int Arm::move(float speedX, float speedY)
{
	//change position
	posX += posDifference * speedX * -1;	//multiplied by -1, because forward motion is in -x direction
	posY += posDifference * speedY;

	//check if position is posible
	if (posPossible(posX, posY)) {
		std::vector<int> newPos = posToAngles(posX, posY, headAngle);
		bool newPosPossible = constraint(newPos, constr_min, constr_max);
		//extra check if position is possible
		if (newPosPossible) {
			for (int i = 0; i < newPos.size(); i++) {
				ax12a.move(servoIDs[i + 1], newPos[i]);
				//std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}

			return 0;
		}
	}

	//undo position change if position NOT possible
	posX -= posDifference * speedX * -1;	//multiplied by -1, because forward motion is in -x direction
	posY -= posDifference * speedY;

	return -1;
}

//speed variable between -1.0 and 1.0
int Arm::turn(float speed)
{
	posRotation += speed * rotDifference;

	//check if rotation position out of range
	if (posRotation < servoMinRotation)
		posRotation = servoMinRotation;
	else if(posRotation > servoMaxRotation)
		posRotation = servoMaxRotation;

	ax12a.move(servoIDs[0], posRotation);
	//std::this_thread::sleep_for(std::chrono::milliseconds(20));

	return 0;
}

void Arm::moveTo(float x, float y, float ha, int rotation)
{
	std::vector<std::vector<int>> path = getPath(posX, posY, x, y, headAngle, ha);

	if (rotation < servoMinRotation)
		rotation = servoMinRotation;
	else if (rotation > servoMaxRotation)
		rotation = servoMaxRotation;

	ax12a.move(servoIDs[0], rotation);

	int pathLength = path.size();
	float speed;
	std::vector<int> currentPos = path[0];

	for (int p = 1; p < pathLength; p++){
		for (int i = 0; i < path[p].size(); i++) {
			int diff = (path[p][i] - currentPos[i]);
			if (diff < 0) diff *= -1;

			float tempVal = (diff / 1227.6) * 600;
			speed = (tempVal * 1023) / 113.553;
			if (speed > 1023) speed = 1023;

			ax12a.moveSpeed(servoIDs[i + 1], path[p][i], (int)speed);
		}
		currentPos = path[p];
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	posX = x;
	posY = y;
	headAngle = ha;
}

void Arm::grab(bool close)
{
	if (close) {
		//close
	}
	else {
		//open
	}
}

float Arm::getPosX()
{
	return posX;
}

float Arm::getPosY()
{
	return posY;
}

float Arm::getHeadAngle()
{
	return headAngle;
}