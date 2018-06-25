#include <thread>
#include <chrono>
#include "InverseKinematics.h"	//also includes: "armConstantes.h"
#include "armConstants.h"
#include "arm.h"

#include <limits>
#include <cstdint>
#ifndef INTMIN
#define INTMIN (-1) //(std::numeric_limits<std::int32_t>::min())
#endif

//debug:
#include <iostream>
#include <thread>
#include <mutex>

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
	mutex = new std::mutex();

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
	bool newPosPossible = constraint(newPos, constr_min.joints, constr_max.joints);

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
	posRotation = (int)(((rotation * 1023) - 1023) * -1); //round(rotation * 1023);
	mutex->lock();
	posRotation = turn(servos.armRotation, posRotation, 200);
	mutex->unlock();
}

std::vector<int> Arm::getArmServoPositions()
{
	std::vector<int> result;

	mutex->lock();
	for (int servoID : servos.joints) {
		result.push_back(ax12a.readPosition(servoID));
	}
	mutex->unlock();

	return result;
}

// delay in milliseconds
int Arm::move(int delay)
{
	//change position
	posX = posX + (maxSpeed * speedX * -1);	//multiplied by -1, because forward motion is in -x direction
	posY = posY + (maxSpeed * speedY);

	//turn arm
	if (speedRotation != 0) {
		posRotation += maxSpeedRotation * speedRotation;
		mutex->lock();
		posRotation = turn(servos.armRotation, posRotation);
		mutex->unlock();
	}

	if (!posPossible(posX, posY)) {
		float vectorSize = sqrt(posX * posX + posY * posY);
		posX = (posX / vectorSize) * ((l1 + l2)*0.9999999);
		posY = (posY / vectorSize) * ((l1 + l2)*0.9999999);
	}

	//std::cout << "load=" << ax12a.readLoad(servos.joints[0]) << std::endl;

	//check if position is posible
	if (posPossible(posX, posY)) {
		std::vector<int> newPos = posToAngles(posX, posY, headAngle);
		bool newPosPossible = constraint(newPos, constr_min.joints, constr_max.joints);
		//extra check if position is possible
		if (newPosPossible) {
			int size = newPos.size();
			for (int i = 0; i < size; i++) {
				int diff = (newPos[i] - currentPosServos[i]);
				mutex->lock();
				turn(servos.joints[i], newPos[i], calcRotationSpeed(diff, delay));
				mutex->unlock();
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

void Arm::setGripperRotation(int position)
{
	mutex->lock();
	turn(servos.gripperRotation, position);
	mutex->unlock();
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

	mutex->lock();
	turn(servos.armRotation, rotation);
	mutex->unlock();

	int pathLength = path.size();

	for (int p = 1; p < pathLength; p++){
		int size = path[p].size();
		for (int i = 0; i < size; i++) {
			int diff = (path[p][i] - currentPosServos[i]);

			mutex->lock();
			turn(servos.joints[i], path[p][i], calcRotationSpeed(diff, moveToDelay));
			mutex->unlock();
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
	mutex->lock();
	if (close) turn(servos.gripper, 710, gripperSpeed);	//close
	else turn(servos.gripper, 900, gripperSpeed);		//open
	mutex->unlock();
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
	mutex->lock();
	return ax12a.readPosition(servos.gripper);
	mutex->unlock();
}


//int Arm::getVoltage() {
//	return ax12a.readVoltage(servos.armRotation);
//}
int Arm::getVoltage() {
	int total = 0;
	int count = 0;
	int temp;
	mutex->lock();
	int size = servos.joints.size();
	mutex->unlock();
	for (int i = 0; i < size; i++) {
		//std::this_thread::sleep_for(std::chrono::milliseconds(5));
		mutex->lock();
		temp = ax12a.readVoltage(servos.joints[i]);
		mutex->unlock();
		if (temp > 90 && temp < 126) { total += temp; count++; }
	}

	return count > 0 ? total/count : -1;

	//std::cout << "voltage=" << total / count << ", total=" << total << ", count=" << count << std::endl;
}
int Arm::getVoltageByID(const int ID) {
	int temp = ax12a.readVoltage(ID);
	if (temp < 90 || temp > 130) { temp = -1; }
	return temp;
}


ArmServos Arm::setServoValues(ArmServos values, int delay) {
	return setServoValues(values, delay, readServoValues());
}


ArmServos Arm::setServoValues(ArmServos values, int delay, ArmServos oldValues) 
{
	constraintServoValues(values, constr_min, constr_max);

	if (values.armRotation == INTMIN) values.armRotation = oldValues.armRotation;
	mutex->lock();
	ax12a.moveSpeed(servos.armRotation, values.armRotation, calcRotationSpeed((oldValues.armRotation - values.armRotation), delay));
	int size = servos.joints.size();
	mutex->unlock();
	for (int i = 0; i < size; i++) {
		if (values.joints[i] == INTMIN) values.joints[i] = oldValues.joints[i];
		mutex->lock();
		ax12a.moveSpeed(servos.joints[i], values.joints[i], calcRotationSpeed((oldValues.joints[i] - values.joints[i]), delay));
		mutex->unlock();
	}
	if (values.gripperRotation == INTMIN) values.gripperRotation = oldValues.gripperRotation;
	mutex->lock();
	ax12a.moveSpeed(servos.gripperRotation, values.gripperRotation, calcRotationSpeed((oldValues.gripperRotation - values.gripperRotation), delay));
	mutex->unlock();
	if (values.gripper == INTMIN) values.gripper = oldValues.gripper;
	mutex->lock();
	ax12a.moveSpeed(servos.gripper, values.gripper, calcRotationSpeed((oldValues.gripper - values.gripper), delay));
	mutex->unlock();
	std::this_thread::sleep_for(std::chrono::milliseconds(delay));

	return values;

	//std::cout << "values are = ";

	//if (values.armRotation == INTMIN) values.armRotation = oldValues.armRotation;
	//std::cout << values.armRotation << ",";
	//int size = values.joints.size();
	//for (int i = 0; i < size; i++) {
	//	if (values.joints[i] == INTMIN) values.joints[i] = oldValues.joints[i];
	//	std::cout << values.joints[i] << ",";
	//}
	//if (values.gripperRotation == INTMIN) values.gripperRotation = oldValues.gripperRotation;
	//std::cout << values.gripperRotation << ",";
	//if (values.gripper == INTMIN) values.gripper = oldValues.gripper;
	//std::cout << values.gripper;

	//std::cout << std::endl;

	return values;
}


bool Arm::constraintServoValues(ArmServos &values, ArmServos constr_min, ArmServos constr_max)
{
	bool result = true;

	//Constraint armRotation
	if (values.armRotation < constr_min.armRotation) {
		values.armRotation = constr_min.armRotation;
		result = false;
	}
	else if (values.armRotation > constr_max.armRotation) {
		values.armRotation = constr_max.armRotation;
		result = false;
	}

	//Constraint all joint servo values
	if (values.joints.size() == constr_min.joints.size() && constr_min.joints.size() == constr_max.joints.size()) {
		int size = values.joints.size();
		for (int i = 0; i < size; i++) {
			if (values.joints[i] < constr_min.joints[i]) {
				values.joints[i] = constr_min.joints[i];
				result = false;
			}
			else if (values.joints[i] > constr_max.joints[i]) {
				values.joints[i] = constr_max.joints[i];
				result = false;
			}
		}
	}
	else {
		std::cout << "WARNING: Input Vector sizes aren't the same. No constraints were done" << std::endl;
		result = false;
	}

	//Constraint gripperRotation
	if (values.gripperRotation < constr_min.gripperRotation) {
		values.gripperRotation = constr_min.gripperRotation;
		result = false;
	}
	else if (values.gripperRotation > constr_max.gripperRotation) {
		values.gripperRotation = constr_max.gripperRotation;
		result = false;
	}

	//Constraint gripper
	if (values.gripper < constr_min.gripper) {
		values.gripper = constr_min.gripper;
		result = false;
	}
	else if (values.gripper > constr_max.gripper) {
		values.gripper = constr_min.gripper;
		result = false;
	}

	return result;
}


ArmServos Arm::readServoValues(bool showWarnings) {
	ArmServos values;
	mutex->lock();
	if ((values.armRotation = ax12a.readPosition(servos.armRotation)) < 0 && showWarnings) std::cout << "WARNING: armRotation value = " << values.armRotation << std::endl;

	int size = servos.joints.size();
	int temp;
	for (int i = 0; i < size; i++) {
		values.joints.push_back(temp = ax12a.readPosition(servos.joints[i]));
		if (temp < 0 && showWarnings) std::cout << "WARNING: joint["<< i <<"] value = " << values.joints[i] << std::endl;;
	}

	if ((values.gripperRotation = ax12a.readPosition(servos.gripperRotation)) < 0 && showWarnings) std::cout << "WARNING: gripperRotation value = " << values.gripperRotation << std::endl;;

	if ((values.gripper = ax12a.readPosition(servos.gripper)) < 0 && showWarnings) std::cout << "WARNING: gripper value = " << values.gripper << std::endl;
	mutex->unlock();

	//std::cout << "oldValues = setServoValues({ " << values.armRotation << ", { " << values.joints[0] << ", " << values.joints[1] << ", " << values.joints[2] << " }, " << values.gripperRotation << ", " << values.gripper << "}, 500, oldValues);" << std::endl;
	//std::cout << "values: " << values.armRotation << ", " << (int)((values.joints[0]+values.joints[1])/2) << ", " << (int)((values.joints[0]+values.joints[1])/2) << ", " << values.joints[2] << ", " << values.joints[3] << ", " << values.gripperRotation << ", " << values.gripper << std::endl;
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


//Without value between commas (",,") value will be INT_MIN
std::vector<std::vector<int>> Arm::CSVtoi(std::string fileName, int nullValue)
{
	std::vector<std::vector<int>> data;
	std::ifstream infile(fileName);
	
	while (infile) {
		std::string s;
		if (!getline(infile, s)) break;

		s = s.substr(0, s.find_first_of(";\r"));
		s.erase(std::remove(s.begin(), s.end(), ' '), s.end());

		if (s != "")
		{
			std::istringstream ss(s);
			std::vector<int> record;

			while (ss)
			{
				std::string s;
				if (!getline(ss, s, ',')) break;
				record.push_back(s != "" ? std::stoi(s) : nullValue);
			}

			data.push_back(record);
		}
	}
	if (!infile.eof()) {
		std::cerr << "Error Reading File From Location " << fileName << std::endl;
	}

	return data;
}
