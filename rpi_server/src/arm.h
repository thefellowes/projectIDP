#ifndef ARM_H
#define ARM_H

#include <vector>
#include <string>
#include "AX12A.h"
#include "armConstants.h"
//#include "readCSV.cpp"
#include <fstream>
#include <sstream>
#include <algorithm>

class Arm {
private:
	AX12A ax12a;
	ArmServos servos;
	bool continueDance;
	bool moveIsActive;
	bool moveInterrupted;
	float speedX;
	float speedY;
	float speedRotation;
	float posX;
	float posY;
	float headAngle;
	int posRotation;
	std::vector<int> currentPosServos;
	bool posPossible(float x, float y);
	int turn(const int servo, int position, int speed=200);
	int calcRotationSpeed(float diff, int ms);
public:
	Arm(AX12A &servoControl, ArmServos servoIDs);
	void startMovement();
	void stopMovement();
	void setSpeed(float xSpeed, float ySpeed, float rotationSpeed = 0);
	void setRotation(float rotation);
	std::vector<int> getArmServoPositions();
	int move(int delay);
	void setGripperRotation(int position);
	void moveTo(float x, float y, float ha);
	void moveTo(float x, float y, float ha, int rotation, bool getCurvedPath=true);
	void grab(bool close);
	float getPosX();
	float getPosY();
	float getHeadAngle();
	int getPosRotation();
	int getPosGripper();
	int getVoltage();

	//int letsGetGroovy(std::string path);
	//void stopGroovin();
	ArmServos setServoValues(ArmServos values, int delay);
	ArmServos setServoValues(ArmServos values, int delay, ArmServos oldValues);
	bool constraintServoValues(ArmServos &values, ArmServos constr_min, ArmServos constr_max);
	ArmServos readServoValues(bool showWarnings=false);

	std::vector<std::vector<int>> CSVtoi(std::string fileName, int nullValue);
};

#endif //ARM_H