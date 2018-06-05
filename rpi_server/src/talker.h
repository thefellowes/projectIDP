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

#include "AX12A.h"

#define SERVERPORT "1312"
#define IPAdress "192.168.1.11"

class Talker{
	private:
		AX12A ax12a;
		bool stop;
		int sockfd;
		struct addrinfo *servinfo, *p;
	public:
		Talker(AX12A &ax12a);
		void sendMessage(const char *message);
		void startTalking();
		void stopTalking();
		~Talker();
};

#endif