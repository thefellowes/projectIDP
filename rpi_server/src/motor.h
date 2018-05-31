#ifndef MOTOR_H
#define MOTOR_H

class Motor {
	private:
		int pwmPin;
		int directionPinA;
		int directionPinB;
		int speed;
		void changeRotationDelay();
	public:
		static const int minSpeed;
		static const int maxSpeed;
		Motor(int pwmPin, int directionPinA, int directionPinB);
		void setClockwise();
		void setCounterClockwise();
		void setSpeed(int speed);
		int getSpeed();
		void stop();
		~Motor();
};

#endif //MOTOR_H