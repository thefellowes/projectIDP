//#include "vision.h"

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
#define IDgripperRotation (1u)
#define IDgripper (12u)
#define ID (3u)
#define ID0 (144u)
#define ID1 (9u)
#define ID2 (61u)
#define Serial "/dev/ttyAMA0" 


int main(void) {
	//Setup for wiringPi to use Broadcom GPIO pin numbers. For explanation and other options check: http://wiringpi.com/reference/setup/.
	wiringPiSetupGpio(); //This function needs to be called with root privileges.

	int pwmPinL = 12;
	int directionPinAL = 16;
	int directionPinBL = 20;
	int pwmPinR = 13;
	int directionPinAR = 6;
	int directionPinBR = 19;

	AX12A ax12a;
	ArmServos servos;
	Motor leftMotor(pwmPinL, directionPinAL, directionPinBL);
	Motor rightMotor(pwmPinR, directionPinAR, directionPinBR);
	TankTracks tankTracks(leftMotor, rightMotor);
	Talker talker(ax12a);

	//std::vector<std::vector<int>> markerValues;
	//markerValues.push_back({ 100, 73, 44, 141, 255, 255 }); //blue
	//markerValues.push_back({ 40, 50, 50, 85, 220, 200 }); //green
	//markerValues.push_back({ 20, 100, 100, 40, 255, 255 }); //yellow
	//markerValues.push_back({ 1, 100, 100, 10, 255, 255 }); //orange
	//markerValues.push_back({ 160, 20, 70, 190, 255, 255 }); //red
	//Vision vision(markerValues);

	servos.armRotation = IDturn;
	servos.gripper = IDgripper;
	servos.gripperRotation = IDgripperRotation;
	servos.joints.push_back(ID);
	servos.joints.push_back(ID0);
	servos.joints.push_back(ID1);
	servos.joints.push_back(ID2);

	ax12a.begin(BaudRate, DirectionPin, Serial);
	
	ax12a.setID(3, 144);
	
	for	(int i = 0; i < 254; i++)
	{
		int x = ax12a.ping(i);
		if (x != -1) std::cout << x << std::endl;
		
	}

	//Arm arm(ax12a, servos);
	//arm.letsGetGroovy();
	//arm.moveTo(-10, 0, 180, 512);
	//std::this_thread::sleep_for(std::chrono::milliseconds(100));
	//arm.moveTo(-17.5, 7.5, 270, 512);
	
	//ArmServos values = arm.readServoValues();

	//debug("Starting threads...");
	//Start processes in seperate threads
	//std::vector<std::thread> threads;
	//threads.push_back(std::thread(listen_t, std::ref(arm), std::ref(tankTracks), std::ref(talker), std::ref(vision)));
	//threads.push_back(std::thread(&Arm::startMovement, std::ref(arm)));
	//threads.push_back(std::thread(&TankTracks::startMotors, std::ref(tankTracks)));
	//threads.push_back(std::thread(&Vision::startVision, std::ref(vision)));
	//threads.push_back(std::thread(&Talker::startTalking, std::ref(talker)));

	//close threads
	//for (auto &thrd : threads)
	//	thrd.join();

	ax12a.end();

	//TODO: Send ShutDown PI Command
	return 0;
}
