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
	nightcoreListener nc_l;
	user_input parsed_input;

	std::atomic<bool> receivedNewData;
	std::atomic<bool> isReceiving;
	std::atomic<bool> armIsMoving;
	std::atomic<bool> armMoveInterrupted;
	std::atomic<bool> tankTrackMoving;
	std::atomic<bool> tankTrackMoveInterrupted;
	std::atomic<bool> checkDancing;
	std::atomic<bool> isDancing;

	std::vector<std::vector<int>> dancePositions;


	//private functions
private:
	void startReceiving();
	void stopReceiving();
	void startArmMove();
	void stopArmMove();
	void startAutoMove();

	void letsGetGroovy();

	//public functions
public:
	Controller(Listener &listener, Talker &talker, Arm &arm, TankTracks &tankTracks, Vision &vision, nightcoreListener &nc_l);
	void begin();
	void stopAll(std::string reason="*no reason found*");
	
	int getBatteryPercentage();

};

#endif