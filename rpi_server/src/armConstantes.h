#include <vector>


//MIN and MAX turn value of servo
const int servoMinRotation = 0;
const int servoMaxRotation = 1023;

//Length of arm parts
const int l1 = 13;
const int l2 = 13;
//Default values where the angles in the real world is 0 (arm laying flat backwards) 
const std::vector<int> defaultValues = { 210,512,512 };
//MIN and MAX constraint values of the servos
const std::vector<int> constr_min = { 210,0,0 };
const std::vector<int> constr_max = { 900,1023,1023 };

const float maxSpeed = 0.175f;	//size to change position
const float maxSpeedRotation = 10;	//size to change rotation

const int moveDelay = 10;
const float moveToStepSize = 0.1f;
const int moveToDelay = (moveToStepSize / maxSpeed * maxSpeedRotation);