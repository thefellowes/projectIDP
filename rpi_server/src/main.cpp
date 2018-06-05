#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <functional>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

#include <wiringPi.h>

#include "dbg.h"
#include "arm.h"
#include "AX12A.h"
#include "listener.h"
#include "motor.h"
#include "tankTracks.h"
#include "talker.h"
#include "cmdmas.h"

#define DirectionPin (18u)
#define BaudRate (1000000ul)
#define IDturn (14u)
#define IDgripperRotation (144u)
#define ID (3u)
#define ID1 (9u)
#define ID2 (61u)
#define Serial "/dev/ttyAMA0" 


int main(void) {
	//Setup for wiringPi to use Broadcom GPIO pin numbers. For explanation and other options check: http://wiringpi.com/reference/setup/.
	wiringPiSetupGpio(); //This function needs to be called with root privileges.

	int pwmPinL = 12;
	int directionPinAL = 5;
	int directionPinBL = 6;
	int pwmPinR = 13;
	int directionPinAR = 19;
	int directionPinBR = 26;

	AX12A ax12a;
	ArmServos servos;
	Motor leftMotor(pwmPinL, directionPinAL, directionPinBL);
	Motor rightMotor(pwmPinR, directionPinAR, directionPinBR);
	TankTracks tankTracks(leftMotor, rightMotor);
	Talker talker(ax12a);

	servos.armRotation = IDturn;
	servos.gripper = 12;//not connected/defined yet
	servos.gripperRotation = IDgripperRotation;
	servos.joints.push_back(ID);
	servos.joints.push_back(ID1);
	servos.joints.push_back(ID2);

	ax12a.begin(BaudRate, DirectionPin, Serial);

	Arm arm(ax12a, servos);
	//arm.letsGetGroovy();
	//arm.moveTo(-10, 0, 180, 512);
	//std::this_thread::sleep_for(std::chrono::milliseconds(100));
	//arm.moveTo(-17.5, 7.5, 270, 512);
	
	//ArmServos values = arm.readServoValues();

	//Start processes in seperate threads
	std::vector<std::thread> threads;
	threads.push_back(std::thread(listen_t, std::ref(arm), std::ref(tankTracks), std::ref(talker)));
	threads.push_back(std::thread(&Arm::startMovement, std::ref(arm)));
	threads.push_back(std::thread(&TankTracks::startMotors, std::ref(tankTracks)));
	threads.push_back(std::thread(&Talker::startTalking, std::ref(talker)));

	//close threads
	for (auto &thrd : threads)
		thrd.join();

	ax12a.end();
	return 0;
}
