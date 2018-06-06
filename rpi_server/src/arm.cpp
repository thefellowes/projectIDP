#include <thread>
#include <chrono>
#include "InverseKinematics.h"	//also includes: "armConstantes.h"
#include "armConstants.h"
#include "arm.h"

//debug:
#include <iostream>

bool Arm::posPossible(float x, float y)
{
	int armlength = l1 + l2;
	return (x * x + y * y > armlength * armlength) ? false : true;
}

int Arm::turn(const int servo, int position, int speed)
{
	//check if rotation position out of range
	position = position < servoMinRotation ? servoMinRotation : position > servoMaxRotation ? servoMaxRotation : position;

	//check if speed out of range
	speed = speed < servoMinSpeed ? servoMinSpeed : speed > servoMaxSpeed ? servoMaxSpeed : speed;

	//turn servo
	ax12a.moveSpeed(servo, position, speed);

	return position;
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

Arm::Arm(AX12A &servoControl, ArmServos servoIDs)
{
	ax12a = servoControl;
	servos = servoIDs;
	moveIsActive = false;
	speedX = 0;
	speedY = 0;
	speedRotation = 0;
	posX = 0;
	posY = l1 + l2;
	posRotation = 512;
	headAngle = 180.0f;
	currentPosServos = getArmServoPositions();

	//set servo's in default position
	posRotation = turn(servos.armRotation, posRotation);
	//std::this_thread::sleep_for(std::chrono::milliseconds(20));

	std::vector<int> newPos = posToAngles(posX, posY, headAngle);
	bool newPosPossible = constraint(newPos, constr_min, constr_max);

	//extra check if position is possible
	if (newPosPossible) {
		int size = newPos.size();
		for (int i = 0; i < size; i++) {
			turn(servos.joints[i], newPos[i], 300);
			//std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}

	std::this_thread::sleep_for(std::chrono::seconds(1));
}

void Arm::startMovement() 
{
	moveIsActive = true;

	while (moveIsActive)
	{
		if (!moveInterrupted)
			move(moveDelay);
		std::this_thread::sleep_for(std::chrono::milliseconds(moveDelay));
	}
	std::cout << "Arm Stopped" << std::endl;
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
// rotation variable between 0.0 and 1.0
void Arm::setRotation(float rotation) {
	posRotation = round(rotation * 1023);
	posRotation = turn(servos.armRotation, posRotation, 200);
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

	//turn arm
	if (speedRotation != 0) {
		posRotation += maxSpeedRotation * speedRotation;
		posRotation = turn(servos.armRotation, posRotation);
	}

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
			int size = newPos.size();
			for (int i = 0; i < size; i++) {
				int diff = (newPos[i] - currentPosServos[i]);
				turn(servos.joints[i], newPos[i], calcRotationSpeed(diff, delay));
			}
			currentPosServos = newPos;

			return 0;
		}
	}

	//undo position change if position NOT possible
	posX -= maxSpeed * speedX * -1;	//multiplied by -1, because forward motion is in -x direction
	posY -= maxSpeed * speedY;

	std::cout << ax12a.readLoad(servos.joints[0]) << std::endl;

	return -1;
}

void Arm::setGripperRotation(int position)
{
	turn(servos.gripperRotation, position);
}
//void Arm::setGripperHorizontal() {
//	turn(servos.gripperRotation, 512);
//}
//void Arm::setGripperVertical() {
//	turn(servos.gripperRotation, 210);
//}

void Arm::moveTo(float x, float y, float ha)
{
	moveTo(x, y, ha, posRotation);
}

void Arm::moveTo(float x, float y, float ha, int rotation, bool getCurvedPath)
{
	moveInterrupted = true;

	std::vector<std::vector<int>> path = getPath(posX, posY, x, y, headAngle, ha, (getCurvedPath ? 5.0f : 45.0f));

	turn(servos.armRotation, rotation);

	int pathLength = path.size();

	for (int p = 1; p < pathLength; p++){
		int size = path[p].size();
		for (int i = 0; i < size; i++) {
			int diff = (path[p][i] - currentPosServos[i]);

			turn(servos.joints[i], path[p][i], calcRotationSpeed(diff, moveToDelay));
		}
		currentPosServos = path[p];
		std::this_thread::sleep_for(std::chrono::milliseconds(moveToDelay));
	}

	posX = x;
	posY = y;
	headAngle = ha;

	moveInterrupted = false;
}

void Arm::grab(bool close)
{
	if (close) turn(servos.gripper, 0, gripperSpeed);	//close
	else turn(servos.gripper, 512, gripperSpeed);		//open
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

//int Arm::getVoltage() {
//	return ax12a.readVoltage(servos.armRotation);
//}
int Arm::getVoltage() {
	int total = 0;
	int count = 0;
	int temp;

	temp = ax12a.readVoltage(servos.armRotation);
	if (temp > 0 && temp < 126) { total += temp; count++; }
	int size = servos.joints.size();
	for (int i = 0; i < size; i++) {
		//std::this_thread::sleep_for(std::chrono::milliseconds(5));
		temp = ax12a.readVoltage(servos.joints[i]);
		if (temp > 0 && temp < 126) { total += temp; count++; }
	}
	//std::this_thread::sleep_for(std::chrono::milliseconds(5));
	temp = ax12a.readVoltage(servos.gripperRotation);
	if (temp > 0 && temp < 126) { total += temp; count++; }
	//std::this_thread::sleep_for(std::chrono::milliseconds(5));
	temp = ax12a.readVoltage(servos.gripper);
	if (temp > 0 && temp < 126) { total += temp; count++; }

	std::cout << "voltage=" << total / count << ", total=" << total << ", count=" << count << std::endl;

	return total / count;
}

void Arm::letsGetGroovy() 
{
	moveInterrupted = true;
	ArmServos oldValues = readServoValues();
		//setServoValues({ rotation, { base joint, mid joint, head joint }, head rotation, gripper }, delay, oldValues);
	//oldValues = setServoValues({ 210, { 470, 748, 820 }, 512, 512 }, 500, oldValues);
	//oldValues = setServoValues({ 210, { 478, 881, 820 }, 512, 512 }, 500, oldValues);
	//oldValues = setServoValues({ 210, { 632, 962, 820 }, 512, 512 }, 500, oldValues);
	//oldValues = setServoValues({ 210, { 446, 763, 820 }, 512, 512 }, 500, oldValues);
	//oldValues = setServoValues({ 210, { 446, 763, 210 }, 512, 512 }, 500, oldValues);
	//oldValues = setServoValues({ 210, { 446, 763, 820 }, 512, 512 }, 500, oldValues);
	//oldValues = setServoValues({ 210, { 446, 763, 210 }, 512, 512 }, 500, oldValues);

	oldValues = setServoValues({ 512,{ 512, 512, 512 }, 512, 512 }, 500, oldValues);
	oldValues = setServoValues({ 512,{ 621, 309, 591 }, 512, 512 }, 1000, oldValues);
	oldValues = setServoValues({ 512,{ 403, 715, 433 }, 512, 512 }, 1000, oldValues);
	oldValues = setServoValues({ 512,{ 621, 309, 591 }, 512, 512 }, 1000, oldValues);
	oldValues = setServoValues({ 512,{ 403, 715, 433 }, 512, 512 }, 1000, oldValues);
	oldValues = setServoValues({ 813,{ 621, 309, 591 }, 512, 512 }, 1000, oldValues);
	oldValues = setServoValues({ 813,{ 403, 715, 433 }, 512, 512 }, 1000, oldValues);
	oldValues = setServoValues({ 813,{ 621, 309, 591 }, 512, 512 }, 1000, oldValues);
	oldValues = setServoValues({ 813,{ 403, 715, 433 }, 512, 512 }, 1000, oldValues);
	oldValues = setServoValues({ 512,{ 512, 512, 512 }, 512, 512 }, 500, oldValues);

	oldValues = setServoValues({ 799, { 591, 309, 429 }, -1, -1}, 500, oldValues);
	oldValues = setServoValues({ 800, { 594, 536, 579 }, -1, -1}, 500, oldValues);
	oldValues = setServoValues({ 799, { 591, 309, 429 }, -1, -1}, 500, oldValues);
	oldValues = setServoValues({ 800, { 594, 536, 579 }, -1, -1}, 500, oldValues);
	oldValues = setServoValues({ 799, { 591, 309, 429 }, -1, -1}, 500, oldValues);
	oldValues = setServoValues({ 800, { 594, 536, 579 }, -1, -1}, 500, oldValues);
	oldValues = setServoValues({ 799, { 591, 309, 429 }, -1, -1}, 500, oldValues);
	oldValues = setServoValues({ 800, { 594, 536, 579 }, -1, -1}, 500, oldValues);
	oldValues = setServoValues({ 799, { 591, 309, 429 }, -1, -1}, 500, oldValues);
	oldValues = setServoValues({ 800, { 594, 536, 579 }, -1, -1}, 500, oldValues);
	
	moveInterrupted = false;
}

ArmServos Arm::setServoValues(ArmServos values, int delay, ArmServos oldValues) 
{
	ax12a.moveSpeed(servos.armRotation, values.armRotation, calcRotationSpeed((oldValues.armRotation - values.armRotation), delay));
	int size = servos.joints.size();
	for (int i = 0; i < size; i++) {
		ax12a.moveSpeed(servos.joints[i], values.joints[i], calcRotationSpeed((oldValues.joints[i] - values.joints[i]), delay));
	}
	ax12a.moveSpeed(servos.gripperRotation, values.gripperRotation, calcRotationSpeed((oldValues.gripperRotation - values.gripperRotation), delay));
	ax12a.moveSpeed(servos.gripper, values.gripper, calcRotationSpeed((oldValues.gripper - values.gripper), delay));
	std::this_thread::sleep_for(std::chrono::milliseconds(delay));

	return values;
}
ArmServos Arm::readServoValues() {
	ArmServos values;

	values.armRotation = ax12a.readPosition(servos.armRotation);
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	int size = servos.joints.size();
	for (int i = 0; i < size; i++) {
		values.joints.push_back(ax12a.readPosition(servos.joints[i]));
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	values.gripperRotation = ax12a.readPosition(servos.gripperRotation);
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	values.gripper = ax12a.readPosition(servos.gripper);
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	std::cout << "oldValues = setServoValues({ " << values.armRotation << ", { " << values.joints[0] << ", " << values.joints[1] << ", " << values.joints[2] << " }, " << values.gripperRotation << ", " << values.gripper << "}, 500, oldValues);" << std::endl;
	//size = servos.joints.size();
	//for (int i = 0; i < size; i++) {
	//	values.joints[i] = ax12a.readPosition(servos.joints[i]);
	//	std::cout << "joint-" << i << " = " << values.joints[i] << std::endl;
	//}
	//std::cout << "gripperRotation = " << values.gripperRotation << std::endl;
	//std::cout << "gripper = " << values.gripper << std::endl;

	return values;
}

std::vector<int> mirrorAnglesOverY(std::vector<int> angles) {
	int size = angles.size();
	for (int i = 0; i < size; i++) {
		angles[i] = (512 - angles[i] + 512);
	}
	return angles;
}