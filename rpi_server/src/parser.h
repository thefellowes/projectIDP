#ifndef __parser_h__
#define __parser_h__

struct user_input parse_input(char** input_data);

struct user_input {
	bool doStop;
	float x;
	float y;
	float a;
	float b;
	float rotation;
	int gripper;
	int dance;
	int lineDance;
	int autoMoveB;
	int autoMoveL;
	int autoMoveO;
	bool checkBattery;
};

#endif
