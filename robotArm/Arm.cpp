#include <thread>
#include <chrono>
//include inversekinematics.h(calculations of next position --> Michiel)

const int servoMinRotation = 0;
const int servoMaxRotation = 1023;

const float Arm::posDifference = 0.5;	//size to change position
const float Arm::rotDifference = 10;	//size to change rotation

bool Arm::posPossible(int x, int y)
{
	int armlength = lenght1 + length2;
	return (x * x + y * y > armlength * armlength) ? false : true;
}

Arm::Arm(AX12a &servoControl, std::vector<int> servoIDs)
{
	ax12a = servos;
	this->servoIDs = servoIDs;
	posX = 0;
	posY = length1 + length2;	//stands in inversekinematics.cpp
	posRotation = 512;
	headAngle = 90.0;

	//set servo's in default position
	ax12.move(servoIDs[0], posRotation);
	std::this_thread::sleep_for(std::chrono::milliseconds(20));

	std::vector<int> newPos = PosToAngles(posX, posY, headAngle, false);
	bool newPosPossible = constraint(newPos, constraint_min, constraint_max); //stands in inversekinematics.cpp

	//extra check if position is possible
	if (newPosPossible) {
		for (int i = 0; i < newPos.size(); i++) {
			ax12a.move(servoIDs[i + 1], newPos[i]);
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}
}

// speedX and speedY variable between -1.0 and 1.0
int Arm::move(float speedX, float speedY)
{
	//change position
	posX += posDifference * speedX * -1;	//multiplied by -1, because forward motion is in -x direction
	posY += posDifference * speedY;

	//check if position is posible
	if (posPossible(posX, posY)) {
		std::vector<int> newPos = PosToAngles(posX, posY, headAngle, false);
		bool newPosPossible = constraint(newPos, constraint_min, constraint_max); //stands in inversekinematics.cpp

		//extra check if position is possible
		if (newPosPossible) {
			for (int i = 0; i < newPos.size(); i++) {
				ax12a.move(servoIDs[i + 1], newPos[i]);
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
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
	std::this_thread::sleep_for(std::chrono::milliseconds(20));

	return 0;
}

void Arm::moveTo(float x, float y, float ha)
{
	std::vector<std::vector<int>> path = getPath(posX, posY, x, y, headAngle, ha);

	for (std::vector<int> position : path) {
		for (int i = 0; i < positon.size(); i++) {
			ax12a.move(servoIDs[i + 1], position);
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}

	//wait to for servo's to reach position
	//TODO:test if delaytime long enough if not change delay time depending on greatest step size
	std::this_thread::sleep_for(std::chrono::milliseconds(200));

	posX = x;
	posY = y;
	headAngle = ha;
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