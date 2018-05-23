#ifndef ARM_H
#define ARM_H

#include <vector>
#include <string>
#include "AX12A.h"

class Arm {
private:
	AX12A ax12a;
	std::vector<int> servoIDs;
	float posX;
	float posY;
	float headAngle;
	int posRotation;
	static const float posDifference;
	static const float rotDifference;
	bool posPossible(int x, int y);
	void turn(int servo, float speed);
	int calcRotationSpeed(float diff, int ms);
public:
	Arm(AX12A &servoControl, std::vector<int> servoIDs);
	int move(float speedX, float speedY);
	void turnArm(float speed);
	void turnGriper(float speed);
	void moveTo(float x, float y, float ha, int rotation=posRotation);
	void grab(bool close);
	float getPosX();
	float getPosY();
	float getHeadAngle();
};

#endif //ARM_H