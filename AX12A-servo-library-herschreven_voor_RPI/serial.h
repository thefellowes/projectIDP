#ifndef SERIAL_H
#define SERIAL_H

#include <queue>

class Serial{
private:
	int fd;
	std::queue<char> buffer;
	bool dataOnSerial();
public:
	int begin(const char *device, const int baud);
	void end();
	void flush();
	void sendData(const unsigned char c);
	void sendData(const char *s);
	void sendData(unsigned char *s, const int length);
	int availableData();
	int readData();
	int peekData();
};

#endif //SERIAL_H