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
	//Calculate percentage of throttle versus direction
	/**float total = positive(throttle) + positive(direction);
	throttle = throttle / total;
	direction = direction / total;

	float percSpeedLeft = throttle + direction;
	float percSpeedRight = throttle - direction;

	speedLeftMotor = leftMotor.maxSpeed * percSpeedLeft;
	speedRightMotor = rightMotor.maxSpeed * percSpeedRight;**/
	
	float maxSpeed = positive(throttle) > positive(direction) ? throttle : direction;
	maxSpeed *= leftMotor.maxSpeed;
	
	//stop motors if both inputs zero and make turn on place if only direction != 0
	if(throttle == 0){
		if(direction == 0) stop();
		else{
			speedLeftMotor = direction * maxSpeed;
			speedRightMotor = -1 * direction * maxSpeed;
		}
	}

	float total = positive(throttle) + positive(direction);
	throttle = throttle / total;
	direction = direction / total;

	float speedY = throttle * maxSpeed;
	float speedX = direction * maxSpeed;

	//calcuate speed for each motor to move in correct direction
	if(throttle > 0 && direction > 0){
		speedLeftMotor = speedY + speedX;
		speedRightMotor = speedY;
	}else if(throttle < 0 && direction < 0){
		speedLeftMotor = speedY;
		speedRightMotor = speedY + speedX;
	}else if(throttle < 0 && direction > 0){
		speedLeftMotor = speedY - speedX;
		speedRightMotor = speedY;
	}else {
		speedLeftMotor = speedY;
		speedRightMotor = speedY - speedX;
	}

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