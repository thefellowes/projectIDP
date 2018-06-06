#ifndef ARM_H
#define ARM_H

#include <vector>
#include <string>
#include "AX12A.h"

struct ArmServos {
	int armRotation;
	std::vector<int> joints;
	int gripperRotation;
	int gripper;
};

class Arm {
private:
	AX12A ax12a;
	ArmServos servos;
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

	void letsGetGroovy();
	ArmServos setServoValues(ArmServos values, int delay, ArmServos oldValues);
	ArmServos readServoValues();

};

#endif //ARM_H