#ifndef __NIGHTCORE_LISTENER_H__
#define __NIGHTCORE_LISTENER_H__

class nightcoreListener {
private:
  int in_pin;
  
public:
  nightcoreListener(int _in_pin);
  int get_in_pin();
  void run();
}

#endif //__NIGHTCORE_LISTENER_H__
