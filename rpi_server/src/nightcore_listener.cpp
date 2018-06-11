#include <wiringPi.h>
#include "nightcore_listener.h"

#define IN_PIN 25

void nc_init() {
  wiringPiSetupGpio();
  pinMode(IN_PIN, INPUT);
}

int get_in_pin() {
  return digitalRead(IN_PIN);
}
