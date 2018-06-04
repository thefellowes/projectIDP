#ifndef TANKTRACKS_H
#define TANKTRACKS_H

#include "motor.h"

class TankTracks {
	private:
		Motor leftMotor;
		Motor rightMotor;
		int speedLeftMotor;
		int speedRightMotor;
		int nextSpeedLeftMotor;
		int nextSpeedRightMotor;
		int speedMax;
		bool motorsRunning;
		static const int acceleration;
		void moveMotors();
		void stop();
	public:
		TankTracks(Motor leftMotor, Motor rightMotor);
		void startMotors();
		void stopMotors();
		void setSpeed(int speedLeft, int speedRight);
		void move(float throttle, float direction, int maxSpeed);
		~TankTracks();
};

#endif //TANKTRACKS_H