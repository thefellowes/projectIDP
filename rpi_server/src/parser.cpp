#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

#define JOY_MIDDLE 512

/*
 * Function to translate the "raw" input from the controller into a value that the pi can handle
 * */
struct user_input parse_input(char** input_data) {
	struct user_input *parsed_input = (struct user_input*) malloc(sizeof(struct user_input));
	float x, y, a, b = 0;
    if(input_data){
		for(int i = 0; *(input_data + i); i++){
    		switch (*(input_data + i)[0]) {
    			case 'x':
					x = atof(*(input_data + i)+1);
					x = ((x - JOY_MIDDLE) / JOY_MIDDLE) * -1;
					if(x < 0.05 && x > -0.05){x=0;}
    				printf("x is:  %f\n", x);
    				break;
    			case 'y':
    				y = atof(*(input_data + i)+1);
					y = (y - JOY_MIDDLE) / JOY_MIDDLE;
					if(y < 0.05 && y > -0.05){y=0;}
    				printf("y is:  %f\n", y);
    				break;
    			case 'a':
    				a = atof(*(input_data + i)+1);
					a = (a - JOY_MIDDLE) / JOY_MIDDLE;
					if(a < 0.05 && a > -0.05){a=0;}
    				printf("a is:  %f\n", a);
    				break;
    			case 'b':
    				b = atof(*(input_data + i)+1);
					b = (b - JOY_MIDDLE) / JOY_MIDDLE;
					if(b < 0.05 && b > -0.05){b=0;}
					printf("b is:  %f\n", b);
    				break;
    			default:
    				printf("unclassified token: %s\n", *(input_data + i));
    		}
    	}	
    }

//    float returnValues [] = {x, y, a, b};
	parsed_input->x = x;
	parsed_input->y = y;
	parsed_input->a = a;
	parsed_input->b = b;
 
 	printf("Return values contains : %f\n", parsed_input->x);
 	printf("Return values contains : %f\n", parsed_input->y);
 	printf("Return values contains : %f\n", parsed_input->a);
 	printf("Return values contains : %f\n", parsed_input->b);
    return *parsed_input;
}
