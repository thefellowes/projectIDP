#ifndef ARM_H
#define ARM_H

#include <vector>
#include <string>
#include "AX12A.h"
#include "armConstantes.h"


class Arm {
private:
	AX12A ax12a;
	std::vector<int> servoIDs;
	bool moveIsActive;
	float speedX;
	float speedY;
	float posX;
	float posY;
	float headAngle;
	int posRotation;
	std::vector<int> currentPosServos;
	bool posPossible(float x, float y);
	void turn(int servo, float speed);
	int calcRotationSpeed(float diff, int ms);
public:
	Arm(AX12A &servoControl, std::vector<int> servoIDs);
	void startMovement();
	void stopMovement();
	void setSpeed(float xSpeed, float ySpeed);
	std::vector<int> getArmServoPositions();
	int move(int delay);
	void turnArm(float speed);
	void turnGriper(float speed);
	void moveTo(float x, float y, float ha);
	void moveTo(float x, float y, float ha, int rotation);
	void grab(bool close);
	float getPosX();
	float getPosY();
	float getHeadAngle();
};

#endif //ARM_H