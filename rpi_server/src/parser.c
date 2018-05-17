#include <stdio.h>
#include <stdlib.h>

#define JOY_MIDDLE 512

int parse_input(char** input_data) {
    if(input_data){
		float x;
		float y;
		float a;
		float b;
    	for(int i = 0; *(input_data + i); i++){
    		switch (*(input_data + i)[0]) {
    			case 'x':
					x = atof(*(input_data + i)+1);
					x = (x - JOY_MIDDLE) / JOY_MIDDLE;
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
    return 0;
}