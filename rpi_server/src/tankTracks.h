#ifndef TANKTRACKS_H
#define TANKTRACKS_H

#include "motor.h"

class TankTracks {
	private:
		Motor leftMotor;
		Motor rightMotor;
		int speedLeftMotor;
		int speedRightMotor;
	public:
		TankTracks(Motor leftMotor, Motor rightMotor);
		void move(float throttle, float direction);
		void stop();
};

#endif //TANKTRACKS_H