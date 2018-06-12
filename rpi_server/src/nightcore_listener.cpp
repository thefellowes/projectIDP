#include <wiringPi.h>
#include <cstdio>
#include <chrono>
#include <thread>

#include "nightcore_listener.h"

nightcoreListener::nightcoreListener(int _in_pin) {
	in_pin = _in_pin;
	wiringPiSetupGpio();
	pinMode(in_pin, INPUT)
}

int get_in_pin() {
  return digitalRead(in_pin);
}

int run() {
	while (1) {
		if (get_in_pin() == HIGH) {
			printf("High AF\n");
			std::chrono::milliseconds timespan(500);
			std::this_thread::sleep_for(timespan);
		}
	}

	return 0;
}
