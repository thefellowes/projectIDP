#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include <string>

#define JOY_MIDDLE 512
#define MAX_ROTATION 1023

/*
 * Function to translate the "raw" input from the controller into a value that the pi can handle
 * */
struct user_input parse_input(char** input_data) {
	struct user_input *parsed_input = (struct user_input*) malloc(sizeof(struct user_input));
	float x, y, a, b = 0;
	float rotation = -1;
	int gripper, dance, lineDance = -1;
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
					x = ((x - JOY_MIDDLE) / JOY_MIDDLE) * -1;
					if(x < 0.05 && x > -0.05){x=0;}
    				//printf("x is:  %f\n", x);
    				break;
    			//Up and down arm
    			case 'y':
    				y = atof(*(input_data + i)+1);
					y = (y - JOY_MIDDLE) / JOY_MIDDLE;
					if(y < 0.05 && y > -0.05){y=0;}
    				//printf("y is:  %f\n", y);
    				break;
    			//Forward and backward movement
    			case 'a':
    				a = atof(*(input_data + i)+1);
					a = (a - JOY_MIDDLE) / JOY_MIDDLE;
					if(a < 0.05 && a > -0.05){a=0;}
    				//printf("a is:  %f\n", a);
    				break;
    			//Left and right movement
    			case 'b':
    				b = atof(*(input_data + i)+1);
					b = ((b - JOY_MIDDLE) / JOY_MIDDLE) * -1;
					if(b < 0.05 && b > -0.05){b=0;}
					//printf("b is:  %f\n", b);
    				break;
				//Rotation arm
    			case 'r':
					rotation = (int)atof(*(input_data + i)+1);
					rotation = rotation / MAX_ROTATION;
    				break;
				//Arm gripper (On/Off)
				case 'G':
					gripper = (int)atof(*(input_data + i) + 1);
					break;
				//Dance (On/Off)
				case 'D':
					dance = (int)atof(*(input_data + i) + 1);
					break;
				//Line-Dance (On/Off)
				case 'L':
					lineDance = (int)atof(*(input_data + i) + 1);
					break;
				case 'B':
					checkBattery = true;
					break;
				case 'A':
					break;
    			default:
    				printf("unclassified token: %s\n", *(input_data + i));
    		}
    	}	
    }

//    float returnValues [] = {x, y, a, b};
	parsed_input->doStop = doStop;
	parsed_input->x = x;
	parsed_input->y = y;
	parsed_input->a = a;
	parsed_input->b = b;
	parsed_input->rotation = rotation;
	parsed_input->gripper = gripper;
	parsed_input->dance = dance;
	parsed_input->lineDance = lineDance;
	parsed_input->checkBattery = checkBattery;
 
 	//printf("Return values contains : %f\n", parsed_input->x);
 	//printf("Return values contains : %f\n", parsed_input->y);
 	//printf("Return values contains : %f\n", parsed_input->a);
 	//printf("Return values contains : %f\n", parsed_input->b);
 	//printf("Return values contains : %f\n", parsed_input->r);
    return *parsed_input;
}
