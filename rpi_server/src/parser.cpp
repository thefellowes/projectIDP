#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include <string>
#include "dbg.h"
#include <iostream>

#define JOY_MIDDLE 512
#define MAX_ROTATION 1023

/*
 * Function to translate the "raw" input from the controller into a value that the pi can handle
 * */
struct user_input parse_input(char** input_data) {
	struct user_input *parsed_input = (struct user_input*) malloc(sizeof(struct user_input));
	float x, y, a, b;
	x = y = a = b = 0;
	float rotation = -1;
	int gripper, dance, lineDance, autoMoveB, autoMoveL, autoMoveO;
	gripper = dance = lineDance = autoMoveB = autoMoveL = autoMoveO = -1;
	bool doStop = false;
	bool checkBattery = false;
	if(input_data){
		for(int i = 0; *(input_data + i); i++){
			switch (*(input_data + i)[0]) {
				//Stop application
				case 'S':
					doStop = true;
					break;
				//Forward and backward arm
				case 'x':
					x = atof(*(input_data + i)+1);
					x = ((x - JOY_MIDDLE) / JOY_MIDDLE);
					if(x < 0.05 && x > -0.05){x=0;}
					break;
				//Up and down arm
				case 'y':
					y = atof(*(input_data + i)+1);
					y = (y - JOY_MIDDLE) / JOY_MIDDLE;
					if(y < 0.05 && y > -0.05){y=0;}
					break;
				//Forward and backward movement
				case 'a':
					a = atof(*(input_data + i)+1);
					a = ((a - JOY_MIDDLE) / JOY_MIDDLE);
					if(a < 0.05 && a > -0.05){a=0;}
					break;
				//Left and right movement
				case 'b':
					b = atof(*(input_data + i)+1);
					b = ((b - JOY_MIDDLE) / JOY_MIDDLE);
					if(b < 0.05 && b > -0.05){b=0;}
					break;
				//Rotation arm
				case 'r':
					rotation = (int)atof(*(input_data + i)+1);
					rotation = rotation / MAX_ROTATION;
					break;
				//Arm gripper (On/Off)
				case 'G':
					gripper = (int)atof(*(input_data + i) + 1);
					std::cout << "G found. Value=" << gripper << std::endl;
					break;
				//Dance (On/Off)
				case 'D':
					dance = (int)atof(*(input_data + i) + 1);
					std::cout << "D found. Value=" << dance << std::endl;
					break;
				//Line-Dance (On/Off)
				case 'L':
					lineDance = (int)atof(*(input_data + i) + 1);
					break;
				case 'T':
					autoMoveB = (int)atof(*(input_data + i) + 1);
					std::cout << "V found. Value=" << autoMoveL << std::endl;
					break;
				//Auto move Follow Line & Catch Balls
				case 'V':
					autoMoveL = (int)atof(*(input_data + i) + 1);
					std::cout << "V found. Value=" << autoMoveL << std::endl;
					break;
				//Auto move Obstacle Course
				case 'F':
					autoMoveO = (int)atof(*(input_data + i) + 1);
					std::cout << "F found. Value=" << autoMoveO << std::endl;
					break;
				//Check battery
				case 'B':
					checkBattery = true;
					std::cout << "B found." << std::endl;
					break;
				case 'A':
					break;
				default:
					log_warn("unclassified token: %s", *(input_data + i));
			}
		}	
	}

	parsed_input->doStop = doStop;
	parsed_input->x = x;
	parsed_input->y = y;
	parsed_input->a = a;
	parsed_input->b = b;
	parsed_input->rotation = rotation;
	parsed_input->gripper = gripper;
	parsed_input->dance = dance;
	parsed_input->lineDance = lineDance;
	parsed_input->autoMoveB = autoMoveB;
	parsed_input->autoMoveL = autoMoveL;
	parsed_input->autoMoveO = autoMoveO;
	parsed_input->checkBattery = checkBattery;

	return *parsed_input;
}

