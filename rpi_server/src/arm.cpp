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
std::vector<int> defaultVal = { 210,512,512 };
std::vector<int> constr_min = { 210,0,0 };
std::vector<int> constr_max = { 900,1023,1023 };

const float Arm::posDifference = 0.5;	//size to change position
const float Arm::rotDifference = 10;	//size to change rotation

float updatesPerSecond = 6;
float delayBetweenUpdates = 1000 / updatesPerSecond;

bool Arm::posPossible(int x, int y)
{
	int armlength = l1 + l2;
	return (x * x + y * y > armlength * armlength) ? false : true;
}

void Arm::turn(int servo, float speed)
{
	posRotation += speed * rotDifference;

	//check if rotation position out of range
	if (posRotation < servoMinRotation)
		posRotation = servoMinRotation;
	else if (posRotation > servoMaxRotation)
		posRotation = servoMaxRotation;

	ax12a.move(servo, posRotation);
	//std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

// diff = difference in position (old_position - new_position)
// ms = the time it has to rotate from the old to new position (in milliseconds)
int Arm::calcRotationSpeed(float diff, int ms) {
	// Make sure the difference is positive
	if (diff < 0) diff *= -1;

	float tempVal = (diff / 1227.6) * (1 / (ms / 1000)) * 60;
	float speed = (tempVal * 1023) / 113.553;
	if (speed > 1023) speed = 1023;

	return roundf(speed);
}

Arm::Arm(AX12A &servoControl, std::vector<int> servoIDs)
{
	ax12a = servoControl;
	this->servoIDs = servoIDs;
	posX = 0;
	posY = l1 + l2;
	posRotation = 512;
	headAngle = 180.0;
	currentPosServos = { 0, 0, 0 };

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

	if (!posPossible(posX, posY)) {
		float vectorSize = sqrt(posX * posX + posY * posY);
		posX = (posX / vectorSize) * ((l1 + l2)*0.9999999);
		posY = (posY / vectorSize) * ((l1 + l2)*0.9999999);
	}

	//check if position is posible
	if (posPossible(posX, posY)) {
		std::vector<int> newPos = posToAngles(posX, posY, headAngle);
		bool newPosPossible = constraint(newPos, constr_min, constr_max);
		//extra check if position is possible
		if (newPosPossible) {
			for (int i = 0; i < newPos.size(); i++) {
				int diff = (newPos[i] - currentPos[i]);
				ax12a.moveSpeed(servoIDs[i + 1], newPos[i], calcRotationSpeed(diff, delayBetweenUpdates));
				//std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
			currentPosServos = newPos;
			return 0;
		}
	}

	//undo position change if position NOT possible
	posX -= posDifference * speedX * -1;	//multiplied by -1, because forward motion is in -x direction
	posY -= posDifference * speedY;

	return -1;
}

void Arm::turnArm(float speed)
{
	turn(servoIDs[0], speed);
}

void Arm::turnGriper(float speed)
{
	turn(servoIDs[4], speed);
}

void Arm::moveTo(float x, float y, float ha, int rotation=posRotation)
{
	std::vector<std::vector<int>> path = getPath(posX, posY, x, y, headAngle, ha);

	if (rotation < servoMinRotation)
		rotation = servoMinRotation;
	else if (rotation > servoMaxRotation)
		rotation = servoMaxRotation;

	ax12a.move(servoIDs[0], rotation);

	int pathLength = path.size();

	for (int p = 1; p < pathLength; p++){
		for (int i = 0; i < path[p].size(); i++) {
			int diff = (path[p][i] - currentPosServos[i]);

			ax12a.moveSpeed(servoIDs[i + 1], path[p][i], calcRotationSpeed(diff, 100));
		}
		currentPosServos = path[p];
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
		//ax12a.moveSpeed(servoIDs[5], ???, 100);
	}
	else {
		//open
		//ax12a.moveSpeed(servoIDs[5], ???, 100);
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