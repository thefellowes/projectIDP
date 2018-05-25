#include <thread>
#include <chrono>
#include "InverseKinematics.h"
#include "arm.h"

//debug:
#include <iostream>

bool Arm::posPossible(float x, float y)
{
	int armlength = l1 + l2;
	return (x * x + y * y > armlength * armlength) ? false : true;
}

void Arm::turn(int servo, int position)
{
	//check if rotation position out of range
	if (position < servoMinRotation)
		position = servoMinRotation;
	else if (position > servoMaxRotation)
		position = servoMaxRotation;

	//turn servo
	ax12a.moveSpeed(servo, position, 200);
}

// diff = difference in position (old_position - new_position)
// ms = the time it has to rotate from the old to new position (in milliseconds)
int Arm::calcRotationSpeed(float diff, int ms)
{
	// Make sure the difference is positive
	if (diff < 0) diff *= -1;

	float tempVal = (diff / 1227.6f) * (1 / ((float)ms / 1000.0f)) * 60.0f;
	float speed = (tempVal * 1023.0f) / 113.553f;
	if (speed > 1023) speed = 1023;

	return roundf(speed);
}

Arm::Arm(AX12A &servoControl, armServos servoIDs)
{
	ax12a = servoControl;
	servos = servoIDs;
	moveIsActive = false;
	speedX = 0;
	speedY = 0;
	posX = 0;
	posY = l1 + l2;
	posRotation = 512;
	headAngle = 180.0f;
	currentPosServos = getArmServoPositions();

	//set servo's in default position
	ax12a.move(servos.armRotation, posRotation);
	//std::this_thread::sleep_for(std::chrono::milliseconds(20));

	std::vector<int> newPos = posToAngles(posX, posY, headAngle);
	bool newPosPossible = constraint(newPos, constr_min, constr_max);

	//extra check if position is possible
	if (newPosPossible) {
		for (int i = 0; i < newPos.size(); i++) {
			ax12a.moveSpeed(servos.joints[i], newPos[i], 300);
			//std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}

	std::this_thread::sleep_for(std::chrono::seconds(1));
}

void Arm::startMovement() 
{
	moveIsActive = true;

	while (true) 
	{
		if (!moveInterrupted)
			move(moveDelay);
		std::this_thread::sleep_for(std::chrono::milliseconds(moveDelay));

		if (!moveIsActive) break;
	}
}

void Arm::stopMovement()
{
	moveIsActive = false;
}

// speedX, speedY and speedRotation variable between -1.0 and 1.0
void Arm::setSpeed(float xSpeed, float ySpeed, float rotationSpeed)
{
	speedX = xSpeed;
	speedY = ySpeed;
	speedRotation = rotationSpeed;
}

std::vector<int> Arm::getArmServoPositions()
{
	std::vector<int> result;

	for (int servoID : servos.joints) {
		result.push_back(ax12a.readPosition(servoID));
	}

	return result;
}

// delay in milliseconds
int Arm::move(int delay)
{	
	//change position
	posX += maxSpeed * speedX * -1;	//multiplied by -1, because forward motion is in -x direction
	posY += maxSpeed * speedY;
	posRotation += maxSpeedRotation * speedRotation;

	//turn arm
	turn(servos.armRotation, posRotation);

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
				int diff = (newPos[i] - currentPosServos[i]);
				ax12a.moveSpeed(servos.joints[i], newPos[i], calcRotationSpeed(diff, delay));
			}
			currentPosServos = newPos;

			return 0;
		}
	}

	//undo position change if position NOT possible
	posX -= maxSpeed * speedX * -1;	//multiplied by -1, because forward motion is in -x direction
	posY -= maxSpeed * speedY;

	return -1;
}

void Arm::setGripperPosition(int position)
{
	turn(servos.gripperRotation, position);
}

void Arm::moveTo(float x, float y, float ha)
{
	moveTo(x, y, ha, posRotation);
}

void Arm::moveTo(float x, float y, float ha, int rotation)
{
	moveInterrupted = true;

	std::vector<std::vector<int>> path = getPath(posX, posY, x, y, headAngle, ha);

	if (rotation < servoMinRotation)
		rotation = servoMinRotation;
	else if (rotation > servoMaxRotation)
		rotation = servoMaxRotation;

	ax12a.move(servos.armRotation, rotation);

	int pathLength = path.size();

	for (int p = 1; p < pathLength; p++){
		for (int i = 0; i < path[p].size(); i++) {
			int diff = (path[p][i] - currentPosServos[i]);

			ax12a.moveSpeed(servos.joints[i], path[p][i], calcRotationSpeed(diff, 100));
		}
		currentPosServos = path[p];
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	posX = x;
	posY = y;
	headAngle = ha;

	moveInterrupted = false;
}

void Arm::grab(bool close)
{
	if (close) {
		//close
		//ax12a.moveSpeed(servos.gripper, ???, 100);
	}
	else {
		//open
		//ax12a.moveSpeed(servos.gripper, ???, 100);
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

int Arm::getPosRotation()
{
	return posRotation;
}

int Arm::getPosGripper()
{
	return ax12a.readPosition(servos.gripper);
}
