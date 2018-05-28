#ifndef MOTOR_H
#define MOTOR_H

class Motor {
	private:
		int pwmPin;
		int directionPinA;
		int directionPinB;
		void changeRotationDelay();
	public:
		const int minSpeed;
		const int maxSpeed;
		Motor(int pwmPin, int directionPinA, int directionPinB);
		void setClockwise();
		void setCounterClockwise();
		void setSpeed(int speed);
		void stop();
};

#endif //MOTOR_H