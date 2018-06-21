#include "opencv2/opencv.hpp"

#include "controller.h"
#include "listener.h"
#include "talker.h"
#include "arm.h"
#include "tankTracks.h"
#include "vision.h"
#include "parser.h"
#include "dbg.h"
#include "nightcore_listener.h"

#define DANCE_PATH "/etc/dancePositions.txt"


Controller::Controller(Listener &listener, Talker &talker, Arm &arm, TankTracks &tankTracks, Vision &vision, nightcoreListener &nc_l) : listener(listener), talker(talker), arm(arm), tankTracks(tankTracks), vision(vision), nc_l(nc_l)
{
	receivedNewData = false;
	armMoveInterrupted = false;
	tankTrackMoveInterrupted = false;
	autoModeBlockTower = false;
	autoModeFindLine = false;
	autoModeIsObstacleCourse = false;
	isDancing = false;
	
	dancePositions = arm.CSVtoi(DANCE_PATH, -1);
}

void Controller::begin()
{
	log_info("Starting threads...");
	//Start processes in seperate threads
	std::vector<std::thread> threads;
	threads.push_back(std::thread(&Controller::startReceiving, this));
	threads.push_back(std::thread(&Controller::startArmMove, this));
	threads.push_back(std::thread(&TankTracks::startMotors, std::ref(tankTracks)));
    threads.push_back(std::thread(&Controller::startAutoMove, this));
	threads.push_back(std::thread(&Controller::letsGetGroovy, this));

	threads.push_back(std::thread(&Vision::startVision, std::ref(vision)));
	//threads.push_back(std::thread(&Talker::startTalking, std::ref(talker)));

	
	
	int batteryPerc = 0;
	int batteryPercBuffer = 0;
	int batteryPercBufferSize = 25;
	int tempInt = 0;
	for (int i = 0; i < batteryPercBufferSize; i++) {
		tempInt = getBatteryPercentage();
		if (tempInt != 0) batteryPercBuffer += tempInt;
		std::this_thread::sleep_for(std::chrono::milliseconds(500 / batteryPercBufferSize));
	}
	batteryPerc = batteryPercBuffer / batteryPercBufferSize;

	while (true) 
	{
		vision.doUpdateFrame = true;
		//Update batteryPercentage
		if ((tempInt = getBatteryPercentage()) != 0) batteryPercBuffer += tempInt - batteryPerc;
		batteryPerc = batteryPercBuffer / batteryPercBufferSize;
		if (batteryPerc < 10 && batteryPerc > 0) {
			stopAll("Battery Percentage = " + batteryPerc);
			break;
		}
		
		if (receivedNewData) {
			//New data is being read. Toggle the boolean to be sure we won't miss an update.
			receivedNewData = false;

			mutex.lock();
			user_input parsedInput = parsed_input;
			mutex.unlock();

			//Parse the token to retrieve the user's input
			//struct user_input parsed_input = parse_input(tokenSwitch);
			
			//Update arm
			if (parsedInput.rotation >= 0)
				arm.setRotation(parsedInput.rotation);
			arm.setSpeed(parsedInput.x, parsedInput.y);

			//Update tankTracks (start == 1 / stop == 0)
			
			//autoMoveBlockTower
			if(parsedInput.autoMoveB == 0){
				std::cout << "Giving up on building the tower" << std::endl;
				autoModeBlockTower = false;
				tankTrackMoveInterrupted = false;
			}
			else if(parsedInput.autoMoveB == 1){
				std::cout << "Trying to build the tower" << std::endl;
				autoModeBlockTower = true;
				autoModeFindLine = autoModeIsObstacleCourse = !autoModeBlockTower;
				tankTrackMoveInterrupted = true;
				vision.doUpdateFrame = true;
			}

			//autoMoveFindLine
			if (parsedInput.autoMoveL == 0) {
				std::cout << "Stopped following the line" << std::endl;
				autoModeFindLine = false;
				tankTrackMoveInterrupted = false;
			}
			else if (parsedInput.autoMoveL == 1) {
				std::cout << "Trying to find the line" << std::endl;
				autoModeFindLine = true;
				autoModeBlockTower = autoModeIsObstacleCourse = !autoModeFindLine;
				tankTrackMoveInterrupted = true;
				vision.doUpdateFrame = true;
			}
			
			//autoMoveObstacleCourse
			if (parsedInput.autoMoveO == 0){
				std::cout << "Stopped running the obstacle course" << std::endl;
				autoModeIsObstacleCourse = false;
				tankTrackMoveInterrupted = false;
			}
			else if (parsedInput.autoMoveO == 1) {
				std::cout << "Running the obstacle course" << std::endl;
				autoModeIsObstacleCourse = true;
				autoModeBlockTower = autoModeFindLine = !autoModeIsObstacleCourse;
				tankTrackMoveInterrupted = true;
				vision.doUpdateFrame = true;
			} 
			
			if (!tankTrackMoveInterrupted) {
				tankTracks.move(parsedInput.a, parsedInput.b, 1023);
			}

							
			//If stop-button is pressed, stop application.
			//TODO: check on which batteryPercentage to shutdown the Pi
			if (parsedInput.doStop) {
				stopAll("Stop Button Pressed");
				break;
			}

			if (parsedInput.checkBattery) {
				const char* battery = std::to_string(batteryPerc).c_str();
				talker.sendMessage(battery);
			}

			if (parsedInput.gripper == 0) { arm.grab(true); }
			else if (parsedInput.gripper == 1) { arm.grab(false); }

			if (parsedInput.dance == 0) {
				std::cout << "Starting Dance" << std::endl; 
				isDancing = true; 
			}
			else if (parsedInput.dance == 1) {
				std::cout << "Stopping Dance" << std::endl; 
				isDancing = false; 
			}

			if (parsed_input.lineDance == 0) {
				nc_l.run();
			}
			else if (parsed_input.lineDance == 1) {
				nc_l.stop_run();
			}
		}

	}

	//close threads
	for (auto &thrd : threads)
		thrd.join();
}
void Controller::stopAll(std::string reason) {
	std::cout << "Stopping Application - Reason: " << reason << std::endl;

	isDancing = false;
	checkDancing = false;
	autoMoveOn = false;

	stopArmMove();
	stopReceiving();
	arm.setServoValues({ 510,{ 200, 200, 924, 689 }, 512, 600 }, 500);
	tankTracks.stopMotors();
	vision.stopVision();
}


//Start Async or in new thread
void Controller::startReceiving() 
{
	isReceiving = true;
	
	while (isReceiving) {
		//tokenSwitch = listener.getToken();
		//if (receivedNewData == false) parsed_input = listener.getParsedInput();
		user_input parsedInput = listener.getParsedInput();
		mutex.lock();
		parsed_input = parsedInput;
		mutex.unlock();
		//else std::chrono::milliseconds(1);
		receivedNewData = true;
	}
	std::cout << "Receiving Stopped" << std::endl;
}
void Controller::stopReceiving() {
	isReceiving = false;
}

void Controller::startAutoMove() {
	autoMoveOn = true;
	std::cout << "Watch me go" << std::endl;

	//red circle where robot has to wait when following a line
	bool foundWaitPoint = false;
	char lastDirection = ' ';
	
	while (autoMoveOn){
		foundWaitPoint = false;
		while (autoModeBlockTower){
			std::cout << "Not implemented yet" << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		while (autoModeFindLine){
			//if waitpoint not found yet, check if its there
			if (!foundWaitPoint) {
				if (vision.find_waitPoint()) {
					//found waitpoint
					foundWaitPoint = true;
					std::cout << "Found my pitstop, waiting.." << std::endl;
					//move forward for some seconds to get in the middle of the circle
					std::this_thread::sleep_for(std::chrono::milliseconds(1000));
					//stop and wait for 30 seconds
					tankTracks.setSpeed(0, 0);
					std::this_thread::sleep_for(std::chrono::seconds(30));
					//start moving and follow the line again
					tankTracks.setSpeed(600, 600);
				}
			}
			//search for line
			char direction = vision.find_line();
			if(direction == 'I'){
				continue;
			}else if(direction == 'F'){
				tankTracks.setSpeed(600,600);
			}else if(direction == 'L'){
				if(lastDirection != 'L'){
					tankTracks.setSpeed(0, 0);
					std::this_thread::sleep_for(std::chrono::milliseconds(500));
					lastDirection = 'L';
				}
				tankTracks.setSpeed(500,-500);
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
			}else if(direction == 'R'){
				if (lastDirection != 'R'){
					tankTracks.setSpeed(0, 0);
					std::this_thread::sleep_for(std::chrono::milliseconds(500));
					lastDirection = 'R';
				}
				tankTracks.setSpeed(-500,500);
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
			}else{
				std::cout << "Tried finding the line, couldnt find it though.." <<std::endl;
				tankTracks.setSpeed(0, 0);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		while (autoModeIsObstacleCourse){
			//cap >> frame;
			//cv::imshow("frame", frame);

			/*if (vision.find_marker_cup()) {
				std::cout << "Found the cup, going in!" << std::endl;
				tankTracks.setSpeed(1023, 1023);
			}
			else {
				std::cout << "Tried searching for the cup, didnt find it though.." << std::endl;
			*/
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			/*if(seeMarkedStairs){
				std::cout << "Not implemented yet" << std::endl;
			}
			else if(seeGap){
				std::cout << "Not implemented yet" << std::endl;
			}
			else if(seeSlope){
				std::cout << "Not implemented yet" << std::endl;
			}
			else if(seeCup){
				std::cout << "Not implemented yet" << std::endl;
			}
			else{
				std::cout << "Not implemented yet" << std::endl;
			}*/
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	std::cout << "Turned off auto pilot mode" << std::endl;
}

void Controller::startArmMove() {
	armIsMoving = true;

	while (armIsMoving)
	{
		if (!armMoveInterrupted)
			arm.move(moveDelay);
		std::this_thread::sleep_for(std::chrono::milliseconds(moveDelay));
	}
	std::cout << "Arm Stopped" << std::endl;
}
void Controller::stopArmMove()
{
	armIsMoving = false;
}

void Controller::letsGetGroovy()
{
	checkDancing = true;

	while (checkDancing)
	{
		if (isDancing)
		{
			armMoveInterrupted = true;
			ArmServos oldValues = arm.readServoValues(true);
			ArmServos originalPosition = oldValues;

			int size = dancePositions.size();
			if (size > 0)
			{
				std::cout << "Dancing has started" << std::endl;
				for (int i = 0; i < size; i++) {
					if (!isDancing) {
						std::cout << "Stop Dance! i=" << i << ", size=" << size - 1 << std::endl;
						break;
					}
					//setServoValues({ rotation, { base joint (1), base joint (2), mid joint, head joint }, head rotation, gripper }, delay, oldValues);
					oldValues = arm.setServoValues({ dancePositions[i][0],{ dancePositions[i][1], dancePositions[i][2], dancePositions[i][3], dancePositions[i][4] }, dancePositions[i][5], dancePositions[i][6] }, dancePositions[i][7], oldValues);
				}

				arm.setServoValues(originalPosition, 500, oldValues);
			}

			isDancing = false;
			armMoveInterrupted = false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}


int Controller::getBatteryPercentage() {
	int result = (int)(((float)arm.getVoltage() - 99) / (126 - 99) * 100);
	result = result > 100 ? 100 : result < 0 ? 0 : result;
	return result;
}
