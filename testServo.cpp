//Compile with: g++ -o testServo testServo.cpp AX12A.cpp serial.cpp -lwiringPi

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <thread>
#include <chrono>

#include "AX12A.h"

#define DirectionPin 	(18u)
#define BaudRate		(1000000ul)
#define ID				(61u)
#define ID1				(12u)
#define ID2				(6u)
#define Serial			"/dev/serial0"	//"/dev/ttyS0"

#define delay(args) 	(std::this_thread::sleep_for(std::chrono::milliseconds(args)))

int main()
{
	int reg, reg1, reg2, ret = 0;
	AX12A ax12a;
	
	//srand (time(NULL));
	
	std::cout << "BaudRate: " << BaudRate << std::endl;
	std::cout << "DirectionPin: " << DirectionPin << std::endl;
	std::cout << "Serial: " << Serial << std::endl;
	
	//std::cout << "return values of move:" << std::endl;

	ax12a.begin(BaudRate, DirectionPin, Serial);

	//int pos1 = 1023;
	//int	pos2 = 0;

	//ax12a.moveSpeed(ID1, pos1, 100);
	//std::this_thread::sleep_for(std::chrono::milliseconds(20));
	//int pos = ax12a.readPosition(ID1);
	//while (pos != pos1) {
	//	pos = ax12a.readPosition(ID1);
	//	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	//}

	//ax12a.moveSpeed(ID1, pos2, 1023);

	ret = ax12a.readVoltage(ID);
	std::cout << "vol: "<< ret << std::endl;

	EndlessMode:
	ax12a.setEndless(ID, true);
	delay(200);
	ax12a.turn(ID, LEFT, 500);
	delay(5000);
	ax12a.turn(ID, LEFT, 0);

	ax12a.setEndless(ID1, true);
	delay(200);
	ax12a.turn(ID1, LEFT, 500);
	delay(5000);
	ax12a.turn(ID1, LEFT, 0);

	ax12a.setEndless(ID2, true);
	delay(200);
	ax12a.turn(ID2, LEFT, 500);
	delay(5000);
	ax12a.turn(ID2, LEFT, 0);
	
	ax12a.end();
	
	return 0;
}
