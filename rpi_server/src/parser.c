#include <stdio.h>


int parse_input(char** input_data) {
    if(input_data){
    	int i;
    	for(i = 0; *(input_data + i); i++){
    		switch (*(input_data + i)[0]) {
    			case 'x':
    				printf("x is:  %s\n", *(input_data + i));
    				break;
    			case 'y':
    				printf("y is:  %s\n", *(input_data + i));
    				break;
    			case 'a':
    				printf("a is:  %s\n", *(input_data + i));
    				break;
    			case 'b':
    				printf("b is:  %s\n", *(input_data + i));
    				break;
    			default:
    				printf("unclassified token: %s\n", *(input_data + i));
    		}
    	}	
    }
    return 0;
}