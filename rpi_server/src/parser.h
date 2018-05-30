#ifndef __parser_h__
#define __parser_h__

struct user_input parse_input(char** input_data);

struct user_input {
	float x;
	float y;
	float a;
	float b;
	float r;
};

#endif
