#include "vision.h"

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

#include "controller.h"
#include "dbg.h"
#include "arm.h"
#include "AX12A.h"
#include "listener.h"
#include "motor.h"
#include "tankTracks.h"
#include "talker.h"
#include "cmdmas.h"

//Define Arm constants
#define DirectionPin (18u)
#define BaudRate (1000000ul)
#define IDturn (14u)
#define IDgripper (12u)
#define IDgripperRotation (7u)
#define ID0 (3u)
#define ID1 (144u)
#define ID2 (9u)
#define ID3 (61u)
#define Serial "/dev/ttyAMA0" 

//Define TankTrack constants
#define PwmPinL (12u)
#define DirectionPinAL (16u)
#define DirectionPinBL (20u)
#define PwmPinR (13u)
#define DirectionPinAR (6u)
#define DirectionPinBR (19u)

//Voltage Servo ID
#define VoltageServoID (6u)

//Define Talker constants
const char* SERVERPORT = "1312";
//const char* IPAddress = "192.168.1.11";

//Define Listener constants
const char* MYPORT = "1313";


int main(void) {
	//Setup for wiringPi to use Broadcom GPIO pin numbers. For explanation and other options check: http://wiringpi.com/reference/setup/.
	wiringPiSetupGpio(); //This function needs to be called with root privileges.

	log_info("Startup - init listener");
	Listener listener(MYPORT);
	log_info("Startup - waiting for IP");
	const char* controllerIP = listener.getIP();
	log_info("Startup - init talker");
	Talker talker(SERVERPORT, controllerIP);

	log_info("Startup - init ax12a");
	AX12A ax12a;
	ax12a.begin(BaudRate, DirectionPin, Serial);

	log_info("Startup - init arm");
	Arm arm(ax12a, { IDturn,{ ID0, ID1, ID2, ID3 }, IDgripperRotation, IDgripper });

	log_info("Startup - init nightcore listener");
	nightcoreListener nc_l(25, arm);

	log_info("Startup - init tank tracks");
	Motor leftMotor(PwmPinL, DirectionPinAL, DirectionPinBL);
	Motor rightMotor(PwmPinR, DirectionPinAR, DirectionPinBR);
	TankTracks tankTracks(leftMotor, rightMotor);

	log_info("Startup - init vision");
	std::vector<std::vector<int>> markerValues;
	markerValues.push_back({ 100, 73, 44, 141, 255, 255 }); //blue
	markerValues.push_back({ 40, 50, 50, 85, 220, 200 }); //green
	markerValues.push_back({ 20, 100, 100, 40, 255, 255 }); //yellow
	markerValues.push_back({ 1, 100, 100, 10, 255, 255 }); //orange
	markerValues.push_back({ 160, 20, 70, 190, 255, 255 }); //red
	Vision vision(markerValues);

	log_info("Startup - init controller");
	Controller controller(listener, talker, arm, tankTracks, vision, nc_l, VoltageServoID);
	controller.begin();

	
	ax12a.end();

	//TODO: Send ShutDown PI Command
	return 0;
}
