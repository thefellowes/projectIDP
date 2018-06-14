#ifndef TALKER_H
#define TALKER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>
#include <chrono>
#include <string>

class Talker{
	//private variables
	private:
		int sockfd;
		struct addrinfo *servinfo, *p;
	//private functions
	private:

	//public functions
	public:
		Talker(const char* serverPort, const char* IPAddress);
		void sendMessage(const char *message);
		~Talker();
};

#endif