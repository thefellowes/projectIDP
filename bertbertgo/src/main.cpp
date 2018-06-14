#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <functional>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

#include "cmdmas.h"
#include "dbg.h"
 
#define PI_IP "192.168.1.17"
#define SERVER_PORT "1313"
#define MAXBUFLEN 100
#define SEND_MESSAGE "D0"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void) {
	// print logo	
	print_bootlogo();

	struct addrinfo *servinfo, *p;
	struct addrinfo hints;
	int rv;
	int sockfd;
		
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	
	if((rv = getaddrinfo(PI_IP, SERVER_PORT, &hints, &servinfo)) != 0){
		log_err("getaddrinfo: %s\n", gai_strerror(rv));
	}
	
	//Go through the results
	for(p = servinfo; p != NULL; p = p->ai_next){
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			log_err("talker: socket");
			continue;
		}
		break;
	}
	
	if(p == NULL){
		log_err("talker: faild to create socket");
	}

	int numbytes;
	for(int i = 0; i < 5; i++){
		if((numbytes = sendto(sockfd, SEND_MESSAGE, strlen(SEND_MESSAGE), 0, p->ai_addr, p->ai_addrlen)) == -1){
			perror("talker: sendto");
			exit(1);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	

	std::this_thread::sleep_for(std::chrono::milliseconds(1500));
	system("mpg321 ~/music/Golden_Earring_-_Radar_Love_-_45_RPM_SHORT_Version.mp3");

	return 0;
}
