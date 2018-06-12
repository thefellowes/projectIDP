#include <wiringPi.h>
#include <cstdio>
#include <chrono>
#include <thread>

#include "nightcore_listener.h"

#define IN_PIN 25


void nc_init() {
  wiringPiSetupGpio();
  pinMode(IN_PIN, INPUT);
}

int get_in_pin() {
  return digitalRead(IN_PIN);
}

int beat_t(void) {
	nc_init();
	while (1) {
		if (get_in_pin() == HIGH) {
			printf("High AF\n");
			std::chrono::milliseconds timespan(500);
			std::this_thread::sleep_for(timespan);
		}
	}

	return 0;
}
