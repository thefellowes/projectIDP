#include <thread>
#include <chrono>
#include <wiringPi.h>
#include "motor.h"

const int Motor::minSpeed = 120;
const int Motor::maxSpeed = 1023;

void Motor::changeRotationDelay()
{
	digitalWrite(directionPinA, LOW);
	digitalWrite(directionPinB, LOW);
	
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

Motor::Motor(int pwmPin, int directionPinA, int directionPinB) : pwmPin(pwmPin), directionPinA(directionPinA), directionPinB(directionPinB)
{
	speed = 0;

	//TODO: setup of wiringpi also done in AX12A.cpp --> do this setup in main
	//Setup for wiringPi to use Broadcom GPIO pin numbers. For explanation and other options check: http://wiringpi.com/reference/setup/.
	wiringPiSetupGpio(); //This function needs to be called with root privileges.

	//setup pwmpin and directionpins
	pinMode(pwmPin, PWM_OUTPUT);
	pinMode(directionPinA, OUTPUT);
	pinMode(directionPinB, OUTPUT);

	pwmSetMode(PWM_MODE_MS);

	//set directionpins low and speed 0
	stop();
}

void Motor::setClockwise()
{
	changeRotationDelay();

	//set direction
	digitalWrite(directionPinA, HIGH);
	digitalWrite(directionPinB, LOW);
}

void Motor::setCounterClockwise()
{
	changeRotationDelay();

	//set direction
	digitalWrite(directionPinA, LOW);
	digitalWrite(directionPinB, HIGH);
}

//speed of the motor from 0 to 1024
void Motor::setSpeed(int speed)
{
	//check if speed out of range
	if(speed < minSpeed) speed = minSpeed;
	else if(speed > maxSpeed) speed = maxSpeed;

	this->speed = speed;

	pwmWrite(pwmPin, speed);
}

int Motor::getSpeed()
{
	return speed;
}

void Motor::stop()
{
	digitalWrite(directionPinA, LOW);
	digitalWrite(directionPinB, LOW);
	pwmWrite(pwmPin, 0);
}

Motor::~Motor()
{
	stop();
}