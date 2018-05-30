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

#include "dbg.h"
#include "arm.h"
#include "AX12A.h"
#include "listener.h"


#define DirectionPin (18u)
#define BaudRate (1000000ul)
#define IDturn (14u)
#define ID (3u)
#define ID1 (9u)
#define ID2 (61u)
#define Serial "/dev/ttyAMA0" 


int main(void) {
	AX12A ax12a;
	ArmServos servos;

	servos.armRotation = IDturn;
	servos.gripperRotation = 13;//not connected/defined yet
	servos.gripperRotation = 13;//not connected/defined yet
	servos.joints.push_back(ID);
	servos.joints.push_back(ID1);
	servos.joints.push_back(ID2);

	ax12a.begin(BaudRate, DirectionPin, Serial);

	Arm arm(ax12a, servos);
	arm.letsGetGroovy();
	//arm.moveTo(-10, 0, 180, 512);
	//std::this_thread::sleep_for(std::chrono::milliseconds(100));
	//arm.moveTo(-17.5, 7.5, 270, 512);

	ArmServos values = arm.readServoValues();

	//std::thread thread_listen(listen_t, std::ref(arm));
	//std::thread thread_armMove(&Arm::startMovement, std::ref(arm));

	//thread_listen.join();
	//thread_armMove.join();
	ax12a.end();
	return 0;
}
