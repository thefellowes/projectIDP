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


int main(void){	
	AX12A ax12a;
	ax12a.begin(BaudRate, DirectionPin, Serial);

	std::vector<int> servos = {IDturn, ID, ID1, ID2};
	
	Arm arm(ax12a, servos);	
	arm.moveTo(-10, 0, 180, 512);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	arm.moveTo(-17.5, 7.5, 270, 512);
	
	std::thread thread_listen(listen_t, arm);
	// std::thread thread_vision(vision_t, arm);
	// std::thread thread_listen(listen_t, arm);

	thread_listen.join();
	ax12a.end();
	return 0;
}
