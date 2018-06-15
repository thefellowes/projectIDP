#include "controller.h"
#include "listener.h"
#include "talker.h"
#include "arm.h"
#include "tankTracks.h"
#include "vision.h"
#include "parser.h"
#include "dbg.h"

#define DANCE_PATH "/home/bert/dev/projectIDP/rpi_server/src/dancePositions.txt"


Controller::Controller(Listener &listener, Talker &talker, Arm &arm, TankTracks &tankTracks, Vision &vision) : listener(listener), talker(talker), arm(arm), tankTracks(tankTracks), vision(vision)
{
	receivedNewData = false;
	armMoveInterrupted = false;
	tankTrackMoveInterrupted = false;
	isDancing = false;
}

void Controller::begin()
{
	std::cout << "Starting threads..." << std::endl;
	//Start processes in seperate threads
	std::vector<std::thread> threads;
	threads.push_back(std::thread(&Controller::startReceiving, this));
	threads.push_back(std::thread(&Controller::startArmMove, this));
	threads.push_back(std::thread(&TankTracks::startMotors, std::ref(tankTracks)));
	threads.push_back(std::thread(&Vision::startVision, std::ref(vision)));
	//threads.push_back(std::thread(&Talker::startTalking, std::ref(talker)));

	int batteryPerc;
	int batteryPercBuffer = 0;
	int batteryPercBufferSize = 25;
	int tempInt;
	for (int i = 0; i < batteryPercBufferSize; i++) {
		tempInt = getBatteryPercentage();
		if (tempInt != 0) batteryPercBuffer += tempInt;
		std::this_thread::sleep_for(std::chrono::milliseconds(500 / batteryPercBufferSize));
	}
	batteryPerc = batteryPercBuffer / batteryPercBufferSize;

	while (true) 
	{
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

			//Parse the token to retrieve the user's input
			//struct user_input parsed_input = parse_input(tokenSwitch);
			
			//Update arm
			if (parsed_input.rotation >= 0)
				arm.setRotation(parsed_input.rotation);
			arm.setSpeed(parsed_input.x, parsed_input.y);

			//Update tankTracks
			if (parsed_input.autoMove == 1) {
				startAutoMove();
			}
			else if (parsed_input.autoMove == 0){
				tankTrackMoveInterrupted = false;
			}
			if (!tankTrackMoveInterrupted) {
				tankTracks.move(parsed_input.a, parsed_input.b, 1023);
			}


			//If stop-button is pressed, stop application.
			//TODO: check on which batteryPercentage to shutdown the Pi
			if (parsed_input.doStop) {
				stopAll("Stop Button Pressed");
				break;
			}

			if (parsed_input.checkBattery) {
				const char* battery = std::to_string(batteryPerc).c_str();
				talker.sendMessage(battery);
			}

			if (parsed_input.gripper == 0) { arm.grab(true); }
			else if (parsed_input.gripper == 1) { arm.grab(false); }

			if (parsed_input.dance == 0) { 
				std::cout << "dance started" << std::endl;
				if (!isDancing) {
					isDancing = true;
					std::string path = DANCE_PATH;
					std::thread groovin(&Controller::letsGetGroovy, this, std::ref(path));
					groovin.detach();
				}
			}
			else if (parsed_input.dance == 1) { std::cout << "dance stopped" << std::endl; stopGroovin(); }

			if (parsed_input.lineDance == 0)
				log_warn("Start LineDance has not been implemented yet");
			else if (parsed_input.lineDance == 1)
				log_warn("Stop LineDance has not been implemented yet");
		}

	}

	//close threads
	for (auto &thrd : threads)
		thrd.join();
}
void Controller::stopAll(std::string reason) {
	std::cout << "Stopping Application - Reason: " << reason << std::endl;

	stopGroovin();
	stopArmMove();
	stopReceiving();
	arm.setServoValues({ 510,{ 200, 200, 924, 689 }, 512, -1 }, 500);
	tankTracks.stopMotors();
	vision.stopVision();
}


void Controller::letsGetGroovy(std::string path) 
{
	std::cout << "in thread" << std::endl;
	armMoveInterrupted = true;
	std::cout << "armMoveInterrupted = true;" << std::endl;
	ArmServos oldValues = arm.readServoValues(true);
	std::cout << "ArmServos oldValues = arm.readServoValues(true);" << std::endl;
	ArmServos originalPosition = oldValues;
	std::cout << "ArmServos originalPosition = oldValues;" << std::endl;

	std::cout << "dance started, reading file" << std::endl;
	std::vector<std::vector<int>> positions = arm.CSVtoi(path, -1);
	int size = positions.size();
	for (int i = 0; i < size; i++) {
		std::cout << "Dancing on: i=" << i << ", size=" << size << std::endl;
		if (!isDancing) {
			std::cout << "Stop Dance! i=" << i << ", size=" << size << std::endl;
			break;
		}
		//setServoValues({ rotation, { base joint (1), base joint (2), mid joint, head joint }, head rotation, gripper }, delay, oldValues);
		oldValues = arm.setServoValues({ positions[i][0],{ positions[i][1], positions[i][2], positions[i][3], positions[i][4] }, positions[i][5], positions[i][6] }, positions[i][7], oldValues);
	}

	arm.setServoValues(originalPosition, 500, oldValues);

	//Usually this function reaches the end by itself, and doesn't get interrupted because the stopGroovin function was called by another function.
	//Therefore it calls the stopGroovin function by itself so that the proper values reset, even if new ones are added to the code.
	stopGroovin();
}
void Controller::stopGroovin() {
	isDancing = false;
	armMoveInterrupted = false;
}


//Start Async or in new thread
void Controller::startReceiving() 
{
	isReceiving = true;
	
	while (isReceiving) {
		//tokenSwitch = listener.getToken();
		//if (receivedNewData == false) parsed_input = listener.getParsedInput();
		parsed_input = listener.getParsedInput();
		//else std::chrono::milliseconds(1);
		receivedNewData = true;
	}
	std::cout << "Receiving Stopped" << std::endl;
}
void Controller::stopReceiving() {
	isReceiving = false;
}

void Controller::startAutoMove() {
	tankTrackMoving = true;
	std::cout << "Watch me go" << std::endl;
	while (tankTrackMoving) {
		if (!tankTrackMoveInterrupted) {
			tankTracks.setSpeed(1023, 1023);
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

int Controller::getBatteryPercentage() {
	int result = (int)(((float)arm.getVoltage() - 99) / (126 - 99) * 100);
	result = result > 100 ? 100 : result < 0 ? 0 : result;
	return result;
}
