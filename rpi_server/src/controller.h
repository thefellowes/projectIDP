#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "opencv2/opencv.hpp"

#include "listener.h"
#include "talker.h"
#include "arm.h"
#include "tankTracks.h"
#include "vision.h"
#include "parser.h"
#include "nightcore_listener.h" 

#include <atomic>
#include <iostream>
#include <mutex>
#include <wiringPi.h>

#define moveDelay (50)

class Controller {
	//private variables
private:
	Listener listener;
	Talker talker;
	Arm arm;
	TankTracks tankTracks;
	Vision vision;
	nightcoreListener nc_l;
	std::mutex mutex;
	user_input parsed_input;

	std::atomic<bool> receivedNewData;
	std::atomic<bool> isReceiving;
	std::atomic<bool> armIsMoving;
	//std::atomic<bool> armMoveInterrupted;
	std::atomic<bool> autoMoveOn;
	std::atomic<bool> autoModeIsLine;
	std::atomic<bool> autoModeBlockTower;
	std::atomic<bool> autoModeFindLine;
	std::atomic<bool> autoModeIsObstacleCourse;
	std::atomic<bool> tankTrackMoveInterrupted;
	//std::atomic<bool> checkDancing;
	std::atomic<bool> isDancing;
	std::atomic<bool> isLineDancing;
	std::atomic<bool> isParcing;

	std::vector<std::vector<int>> dancePositions;

	int vsID;

	//private functions
private:
	void startReceiving();
	void stopReceiving();
	void startArmMove();
	void stopArmMove();
	void startAutoMove();

	//void letsGetGroovy();

	//public functions
public:
	Controller(Listener &listener, Talker &talker, Arm &arm, TankTracks &tankTracks, Vision &vision, nightcoreListener &nc_l, const int &VoltageServoID);
	void begin();
	void stopAll(std::string reason="*no reason found*");
	
	int getBatteryPercentage();

};

#endif
