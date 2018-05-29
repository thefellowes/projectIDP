#include "tankTracks.h"

template <typename T>
T positive(T x) {
	return x < 0 ? x*-1 : x;
}

TankTracks::TankTracks(Motor leftMotor, Motor rightMotor)
{
	this->leftMotor = leftMotor;
	this->rightMotor = rightMotor;
	speedLeftMotor, speedRightMotor = 0;
	moveIsActive = false;
}

//throttle and direction variable between -1.0 and 1.0
void TankTracks::move(float throttle, float direction)
{
	//stop motors if no motion zero
	if(throttle == 0 && direction == 0) stop();
	
	//TODO: check if one motor on and one off possible (probably not)
	
	//Calculate percentage of throttle versus direction
	float total = positive(throttle) + positive(direction);
	throttle = throttle / total;
	direction = direction / total;

	float percSpeedLeft = throttle + direction;
	float percSpeedRight = throttle - direction;

	speedLeftMotor = leftMotor.maxSpeed * percSpeedLeft;
	speedRightMotor = rightMotor.maxSpeed * percSpeedRight;

	//Check in which direction motor has to turn and set speed
	if(speedLeftMotor < 0){
		leftMotor.setCounterClockwise();
		leftMotor.setSpeed(speedLeftMotor * -1);
	} else{
		leftMotor.setClockwise();
		leftMotor.setSpeed(speedLeftMotor);
	}

	if(speedRightMotor < 0){
		rightMotor.setCounterClockwise();
		rightMotor.setSpeed(speedRightMotor * -1);
	} else{
		rightMotor.setClockwise();
		rightMotor.setSpeed(speedRightMotor);
	}
}

void TankTracks::stop()
{
	leftMotor.stop();
	rightMotor.stop();
}