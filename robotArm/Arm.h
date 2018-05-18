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
public:
	Arm(AX12A &servoControl, std::vector<int> servoIDs);
	int move(float speedX, float speedY);
	int turn(float speed);
	void moveTo(float x, float y, float ha, int rotation);
	//void movePath(std::vector<std::vector<float>> path);
	void grab(bool close);
	float getPosX();
	float getPosY();
	float getHeadAngle();
};

#endif //ARM_H