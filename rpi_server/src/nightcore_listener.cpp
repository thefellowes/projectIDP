#include <wiringPi.h>
#include <cstdio>
#include <chrono>
#include <thread>

#include "arm.h"
#include "nightcore_listener.h"

nightcoreListener::nightcoreListener(int _in_pin, Arm &arm) : arm(arm)
{
	in_pin = _in_pin;
	wiringPiSetupGpio();
	pinMode(in_pin, INPUT);
	stop = 0;
}

int nightcoreListener::get_in_pin() {
  return digitalRead(in_pin);
}

//void nightcoreListener::run() {
//	ArmServos oldValues = arm.readServoValues(true);
//	oldValues = arm.setServoValues({ 512,{ 512, 512, 512, 512 }, 512, 512 }, 500, oldValues);
//	
//	while (!stop) {
//		if (get_in_pin() == HIGH) {
//			//setServoValues({ rotation, { base joint (1), base joint (2), mid joint, head joint }, head rotation, gripper }, delay, oldValues);
//			oldValues = arm.setServoValues({ 512,{ 512, 512, 512, 650 }, 512, 512 }, 250, oldValues);
//			oldValues = arm.setServoValues({ 512,{ 512, 512, 512, 780 }, 512, 512 }, 250, oldValues);
//		}
//	}
//}
//
//void nightcoreListener::stop_run() {
//	stop = 1;
//} 

