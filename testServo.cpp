//Compile with: g++ -o testServo testServo.cpp AX12A.cpp -lwiringPi

#include "AX12A.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

#define DirectionPin 	(8u)
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
	
	srand (time(NULL));
	
	std::cout << "BaudRate: " << BaudRate << std::endl;
	std::cout << "DirectionPin: " << DirectionPin << std::endl;
	std::cout << "Serial: " << Serial << std::endl;
	
	//std::cout << "return values of move:" << std::endl;

	ax12a.begin(BaudRate, DirectionPin, Serial);
	ax12a.setEndless(ID, OFF);
 	ret = ax12a.move(ID,512);
	std::cout << ID << ": " << ret << std::endl;
	delay(200);
	/* ret = ax12a.move(ID2,512);
	std::cout << ID2 << ": " << ret << std::endl;
	delay(200);
	ret = ax12a.move(ID1,512);
	std::cout << ID1 << ": " << ret << std::endl;
	delay(200); */
	
	//ping
	/* ret = ax12a.ping(ID);
	std::cout << "Ping: " << ret << std::endl;
	delay(200); */

	/* for(int i = 0; i < 2; i++)
	{
		std::cout << "return values of move:" << std::endl;
		
		ret = ax12a.move(ID, rand() % 1023);//random(0,1023)
		std::cout << ID << ": " << ret << std::endl;
		delay(300);
		ret = ax12a.move(ID1, rand() % 512);//random(0,512)
		std::cout << ID1 << ": " << ret << std::endl;
		delay(300);
		ret = ax12a.move(ID2, 512 + rand() % (1020-512));//random(512,1020)
		std::cout << ID2 << ": " << ret << std::endl;
		delay(300);
		
		std::cout << "readPositions:" << std::endl;

		reg = ax12a.readPosition(ID);
		reg1 = ax12a.readPosition(ID1);
		reg2 = ax12a.readPosition(ID2);

		std::cout << reg << std::endl;
		std::cout << reg1 << std::endl;
		std::cout << reg2 << std::endl;
	} */
	
	ax12a.end();
	
	return 0;
}