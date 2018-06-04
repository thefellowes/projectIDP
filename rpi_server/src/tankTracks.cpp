#include <thread>
#include <chrono>
#include "tankTracks.h"

//for debug:
#include <iostream>

template <typename T>
T positive(T x) {
	return x < 0 ? x*-1 : x;
}

const int TankTracks::acceleration = 200;

void TankTracks::moveMotors()
{	
	//Check in which direction motor has to turn and set speed
	if(speedLeftMotor < 0){
		leftMotor.setCounterClockwise();
		leftMotor.setSpeed(speedLeftMotor * -1);
	} else if(speedLeftMotor > 0){
		leftMotor.setClockwise();
		leftMotor.setSpeed(speedLeftMotor);
	}else{
		leftMotor.stop();
	}

	if(speedRightMotor < 0){
		rightMotor.setCounterClockwise();
		rightMotor.setSpeed(speedRightMotor * -1);
	} else if(speedRightMotor > 0){
		rightMotor.setClockwise();
		rightMotor.setSpeed(speedRightMotor);
	}else{
		rightMotor.stop();
	}
}

void TankTracks::stop()
{
	speedLeftMotor = 0;
	speedRightMotor = 0;
	nextSpeedLeftMotor = 0;
	nextSpeedRightMotor = 0;
	leftMotor.stop();
	rightMotor.stop();
}

TankTracks::TankTracks(Motor leftMotor, Motor rightMotor) : leftMotor(leftMotor), rightMotor(rightMotor)
{
	speedLeftMotor = 0;
	speedRightMotor = 0;
	nextSpeedLeftMotor = 0;
	nextSpeedRightMotor = 0;
	speedMax = leftMotor.maxSpeed;
	motorsRunning = false;
}

//start this method in different thread
void TankTracks::startMotors()
{
	motorsRunning = true;

	int oldSpeedLeftMotor;
	int oldSpeedRightMotor;

	while (motorsRunning)
	{
		oldSpeedLeftMotor = speedLeftMotor;
		oldSpeedRightMotor = speedRightMotor;

		if(nextSpeedLeftMotor > speedLeftMotor && nextSpeedLeftMotor > speedLeftMotor + acceleration){
			speedLeftMotor += acceleration;
		}else if(nextSpeedLeftMotor < speedLeftMotor && nextSpeedLeftMotor < speedLeftMotor - acceleration){
			speedLeftMotor -= acceleration;
		}else{
			speedLeftMotor = nextSpeedLeftMotor;
		}
		
		if(nextSpeedRightMotor > speedRightMotor && nextSpeedRightMotor > speedRightMotor + acceleration){
			speedRightMotor += acceleration;
		}else if(nextSpeedRightMotor < speedRightMotor && nextSpeedRightMotor < speedRightMotor - acceleration){
			speedRightMotor -= acceleration;
		}else{
			speedRightMotor = nextSpeedRightMotor;
		}


		if (oldSpeedLeftMotor != speedLeftMotor || oldSpeedRightMotor != speedRightMotor) {
			moveMotors();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	std::cout << "tankTracks Stopped" << std::endl;
}

void TankTracks::stopMotors()
{
	motorsRunning = false;
	stop();
}

void TankTracks::setSpeed(int speedLeft, int speedRight)
{
	//check if speed of motors not lower than minimum speed
	if (speedLeft  != 0) speedLeft  = positive(speedLeft) < leftMotor.minSpeed ? (speedLeft/positive(speedLeft)) * leftMotor.minSpeed : speedLeft;
	if (speedRight != 0) speedRight = positive(speedRight) < rightMotor.minSpeed ? (speedRight/positive(speedRight)) * rightMotor.minSpeed : speedRight;

	//for debug:
	//std::cout << "speedLeftMotor: " << speedLeft << std::endl;
	//std::cout << "speedRightMotor: " << speedRight << std::endl;
	
	nextSpeedLeftMotor = speedLeft;
	nextSpeedRightMotor = speedRight;
}

//throttle and direction variable between -1.0 and 1.0
void TankTracks::move(float throttle, float direction, int maxSpeed)
{	
	speedMax = maxSpeed;
	int speedL = speedLeftMotor;
	int speedR = speedRightMotor;

	//set throttle and direction between -1 and 1 if it's not
	throttle = throttle > 1 ? 1 : throttle < -1 ? -1 : throttle;
	direction = direction > 1 ? 1 : direction < -1 ? -1 : direction;
	
	//calcuate the highest speed
	float speed = (positive(throttle) > positive(direction) ? positive(throttle) : positive(direction)) * speedMax;
	
	//stop motors if both inputs in stop range; make turn on place if direction not equal to 0
	if(throttle > -0.1 && throttle < 0.1){
		if(direction == 0){
			speedL = 0;
			speedR = 0;
		}
		else{
			speedL = direction * speed;
			speedR = -1 * direction * speed;
		}
	}else{
		//Calculate percentage of throttle versus direction
		float total = positive(throttle) + positive(direction);
		throttle = throttle / total;
		direction = direction / total;

		float speedY = throttle * speed;
		float speedX = direction * speed;

		//calcuate speed for each motor to move in correct direction
		if(throttle > 0 && direction >= 0){
			speedL = speedY + speedX;
			speedR = speedY;
		}else if(throttle < 0 && direction < 0){
			speedL = speedY;
			speedR = (speedY + speedX);
		}else if(throttle < 0 && direction > 0){
			speedL = speedY - speedX;
			speedR = speedY;
		}else {
			speedL = speedY;
			speedR = (speedY - speedX);
		}
	}

	setSpeed(speedL, speedR);
}

TankTracks::~TankTracks()
{
	stopMotors();
}