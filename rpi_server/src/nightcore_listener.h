#ifndef __NIGHTCORE_LISTENER_H__
#define __NIGHTCORE_LISTENER_H__

#include "arm.h"

class nightcoreListener {
	private:
		int in_pin;
		int stop;
		Arm arm;
  
	public:
		nightcoreListener(int _in_pin, Arm &arm);
		int get_in_pin();
		//void run();
		//void stop_run();
};

#endif //__NIGHTCORE_LISTENER_H__
