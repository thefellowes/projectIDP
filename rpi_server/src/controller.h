#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "listener.h"
#include "talker.h"
#include "arm.h"
#include "tankTracks.h"
#include "vision.h"
#include "parser.h"

#include <atomic>
#include <iostream>
#include <future>

#define moveDelay (10)

class Controller {
	//private variables
private:
	Listener listener;
	Talker talker;
	Arm arm;
	TankTracks tankTracks;
	Vision vision;

	//char** tokenSwitch;
	struct user_input parsed_input;

	std::atomic<bool> receivedNewData;
	bool isReceiving;
	bool armIsMoving;
	bool armMoveInterrupted;
	bool isDancing;

	std::future<void> danceFuture;


	//private functions
private:
	void startReceiving();
	void stopReceiving();
	void startArmMove();
	void stopArmMove();
	void startAutoMove();

	void letsGetGroovy(std::string path);
	void stopGroovin();
	//public functions
public:
	Controller(Listener &listener, Talker &talker, Arm &arm, TankTracks &tankTracks, Vision &vision);
	void begin();
	
	int getBatteryPercentage();

};

#endif