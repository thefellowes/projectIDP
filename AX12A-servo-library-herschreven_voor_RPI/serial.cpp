#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <chrono>
#include <thread>

#include "serial.h"

bool Serial::dataOnSerial()
{
	int number;

	if (ioctl(fd, FIONREAD, &number) == -1)
		return false;

	return number > 0 ? true : false;
}

int Serial::begin(const char *device, const int baud)
{
	struct termios options;
	speed_t myBaud;
	int     status;

	switch (baud)
	{
		case      50:	myBaud = B50; break;
		case      75:	myBaud = B75; break;
		case     110:	myBaud = B110; break;
		case     134:	myBaud = B134; break;
		case     150:	myBaud = B150; break;
		case     200:	myBaud = B200; break;
		case     300:	myBaud = B300; break;
		case     600:	myBaud = B600; break;
		case    1200:	myBaud = B1200; break;
		case    1800:	myBaud = B1800; break;
		case    2400:	myBaud = B2400; break;
		case    4800:	myBaud = B4800; break;
		case    9600:	myBaud = B9600; break;
		case   19200:	myBaud = B19200; break;
		case   38400:	myBaud = B38400; break;
		case   57600:	myBaud = B57600; break;
		case  115200:	myBaud = B115200; break;
		case  230400:	myBaud = B230400; break;
		case  460800:	myBaud = B460800; break;
		case  500000:	myBaud = B500000; break;
		case  576000:	myBaud = B576000; break;
		case  921600:	myBaud = B921600; break;
		case 1000000:	myBaud = B1000000; break;
		case 1152000:	myBaud = B1152000; break;
		case 1500000:	myBaud = B1500000; break;
		case 2000000:	myBaud = B2000000; break;
		case 2500000:	myBaud = B2500000; break;
		case 3000000:	myBaud = B3000000; break;
		case 3500000:	myBaud = B3500000; break;
		case 4000000:	myBaud = B4000000; break;

		default:
			return -2;
	}

	if ((fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) == -1)
		return -1;

	fcntl(fd, F_SETFL, O_RDWR);

	tcgetattr(fd, &options);

	cfmakeraw(&options);
	cfsetispeed(&options, myBaud);
	cfsetospeed(&options, myBaud);

	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag &= ~OPOST;

	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 100;	// Ten seconds (100 deciseconds)

	tcsetattr(fd, TCSANOW, &options);

	ioctl(fd, TIOCMGET, &status);

	status |= TIOCM_DTR;
	status |= TIOCM_RTS;

	ioctl(fd, TIOCMSET, &status);

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	return fd;
}

void Serial::end()
{
	close(fd);
}

void Serial::flush()
{
	tcflush(fd, TCIOFLUSH);			//flush serial
	std::queue<char>().swap(buffer); //clear buffer
}

void Serial::sendData(const unsigned char c)
{
	write(fd, &c, 1);
}

void Serial::sendData(const char *s)
{
	write(fd, s, strlen(s));
}

void Serial::sendData(unsigned char *s, const int length)
{
	write(fd, s, length);
}

int Serial::availableData()
{
	uint8_t c;

	while (dataOnSerial()) {
		if (read(fd, &c, 1) == 1) {
			buffer.push(((int)c) & 0xFF);
		}
	}

	return buffer.size();
}

int Serial::readData()
{
	if (availableData() > 0) {
		int c = buffer.front();
		buffer.pop();
		return c;
	}
	
	return -1;
}

int Serial::peekData()
{
	if (availableData() > 0) {
		return buffer.front();
	}

	return -1;
}