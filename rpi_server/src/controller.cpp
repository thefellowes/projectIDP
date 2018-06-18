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

	dancePositions = arm.CSVtoi(DANCE_PATH, -1);
}

void Controller::begin()
{
	std::cout << "Starting threads..." << std::endl;
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
				tankTrackMoveInterrupted = false;
			}
			else if (parsed_input.autoMove == 0){
				tankTrackMoveInterrupted = true;
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
				std::cout << "Starting Dance" << std::endl; 
				isDancing = true; 
			}
			else if (parsed_input.dance == 1) { 
				std::cout << "Stopping Dance" << std::endl; 
				isDancing = false; 
			}

			if (parsed_input.lineDance == 0) {
				log_warn("Start LineDance has not been implemented yet");
			}
			else if (parsed_input.lineDance == 1) {
				log_warn("Stop LineDance has not been implemented yet");
			}

			if (parsed_input.autoMoveO == 0) {
				log_warn("Start autoMoveObstacleCourse has not been implemented yet");
			}
			else if (parsed_input.autoMoveO == 1) {
				log_warn("Stop autoMoveObstacleCourse has not been implemented yet");
			}

			if (parsed_input.autoMoveL == 0) {
				log_warn("Start autoMoveLine has not been implemented yet");
			}
			else if (parsed_input.autoMoveL == 1) {
				log_warn("Stop autoMoveLine has not been implemented yet");
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
            if(Vision.find_marker_cup()){
                std::cout << "Found the cup, going in!" << std::endl;
                tankTracks.setSpeed(1023, 1023);   
            }
            else{
                std::cout << "Tried searching for the cup, didnt find it though.." << std::endl;
            }
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
