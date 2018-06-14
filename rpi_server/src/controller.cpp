#include "controller.h"

#include "listener.h"
#include "talker.h"
#include "arm.h"
#include "tankTracks.h"
#include "vision.h"
#include "parser.h"

Controller::Controller(Listener &listener, Talker &talker, Arm &arm, TankTracks &tankTracks, Vision &vision) : listener(listener), talker(talker), arm(arm), tankTracks(tankTracks), vision(vision)
{
	receivedNewData = false;
	armMoveInterrupted = false;
	tankTrackMoveInterrupted = false;
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
	threads.push_back(std::thread(&Controller::startAutoMove, this));
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
			if (parsed_input.autoMove == 0) {
				startAutoMove();
			}
			else if (parsed_input.autoMove == 1){
				tankTrackMoveInterrupted = false;
			}
			if (!tankTrackMoveInterrupted) {
				tankTracks.move(parsed_input.a, parsed_input.b, 1023);
			}


			//If batteryPercentage too low shutdown pi
			//TODO: check on which batteryPercentage to shutdown the Pi
			if ((batteryPerc < 10 && batteryPerc > 0) || parsed_input.doStop == true) {
				std::cout << "Stopping Application - Reason: ";
				if (parsed_input.doStop == true) std::cout << "doStop = true" << std::endl;
				if (batteryPerc < 10 && batteryPerc > 0) std::cout << "batteryPerc < 10" << std::endl;

				arm.stopGroovin();
				//arm.stopMovement();
				stopArmMove();
				stopReceiving();
				arm.setServoValues({ 510,{ 200, 200, 924, 689 }, 512, -1 }, 500);
				tankTracks.stopMotors();
				vision.stopVision();


				break;
			}

			if (parsed_input.checkBattery) {
				const char* battery = std::to_string(batteryPerc).c_str();
				talker.sendMessage(battery);
			}

			if (parsed_input.gripper == 0) { arm.grab(true); }
			else if (parsed_input.gripper == 1) { arm.grab(false); }

			//if (parsed_input.dance == 0) { arm.letsGetGroov("/home/bert/dev/projectIDP/rpi_server/src/dancePositions.txt"); }
			if (parsed_input.dance == 0) { 
				std::string path = "/home/bert/dev/projectIDP/rpi_server/src/dancePositions.txt";
				std::future<void> danceFuture2 = std::async(std::launch::async, &Controller::letsGetGroovy, this, std::ref(path)); 
				danceFuture2.wait();
			}
			else if (parsed_input.dance == 1) { std::cout << "Stop Dance has not been implemented yet" << std::endl; }

			if (parsed_input.lineDance == 0) { std::cout << "Start LineDance has not been implemented yet" << std::endl; }
			else if (parsed_input.lineDance == 1) { std::cout << "Stop LineDance has not been implemented yet" << std::endl; }
		}

	}

	//close threads
	for (auto &thrd : threads)
		thrd.join();
}


void Controller::letsGetGroovy(std::string path) 
{
	isDancing = armMoveInterrupted = true;
	ArmServos oldValues = arm.readServoValues(true);
	ArmServos originalPosition = oldValues;

	std::vector<std::vector<int>> positions = arm.CSVtoi(path, -1);
	int size = positions.size();
	for (int i = 0; i < size; i++) {
		if (!isDancing) {
			std::cout << "Stop Dance! i=" << i << ", size=" << size << std::endl;
			break;
		}
		//setServoValues({ rotation, { base joint (1), base joint (2), mid joint, head joint }, head rotation, gripper }, delay, oldValues);
		oldValues = arm.setServoValues({ positions[i][0],{ positions[i][1], positions[i][2], positions[i][3], positions[i][4] }, positions[i][5], positions[i][6] }, positions[i][7], oldValues);
	}

	if (isDancing) arm.setServoValues(originalPosition, 500, oldValues);

	stopGroovin();
}


void Controller::stopGroovin() {
	isDancing = armMoveInterrupted = false;
	danceFuture.get();
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