#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <thread>
#include <functional>
#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <assert.h>

#include "parser.h"
#include "arm.h"
#include "tankTracks.h"
#include "talker.h"

#define MYPORT "1313"
#define MAXBUFLEN 100


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = (char**) malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

int getBatteryPercentage(Arm &arm) {
	int result = (int)(((float)arm.getVoltage() - 99) / (126 - 99) * 100);
	result = result > 100 ? 100 : result < 0 ? 0 : result;
	return result;
}

void listen_t(Arm &arm, TankTracks &tankTracks, Talker &talker) {
	int batteryPerc;
	int batteryPercBuffer = 0;
	int batteryPercBufferSize = 25;
	int tempInt;
	for (int i = 0; i < batteryPercBufferSize; i++) {
		tempInt = getBatteryPercentage(arm);
		if (tempInt != 0) batteryPercBuffer += tempInt;
		std::this_thread::sleep_for(std::chrono::milliseconds(500/batteryPercBufferSize));
	}
	batteryPerc = batteryPercBuffer / batteryPercBufferSize;

	int sockfd;
	//bool rotating = false;
	//float rotSpeed = 0;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	char buf[MAXBUFLEN];
	socklen_t addr_len;
	//char s[INET6_ADDRSTRLEN];
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP
	if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(EXIT_FAILURE);
	}
	
	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
		p->ai_protocol)) == -1) {	
			perror("listener: socket");
			continue;
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "listener: failed to bind socket\n");
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(servinfo);
	printf("listener: waiting to recvfrom...\n");
	while (1) {
		addr_len = sizeof their_addr;
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) == -1) {
			perror("recvfrom");
			exit(1);
		}
		//printf("listener: got packet from %s\n",
		//inet_ntop(their_addr.ss_family,
		//get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
		//printf("listener: packet is %d bytes long\n", numbytes);
		buf[numbytes] = '\0';

		char** tokenSwitch;
		tokenSwitch = str_split(buf, '\n');

		//X, Y, A, B
		struct user_input parsed_input = parse_input(tokenSwitch);
		//printf("Move : %f |  %f \n", parsed_input.x, parsed_input.y);
		//printf("Rotation is : %f\n", parsed_input.r);
		if (parsed_input.rotation >= 0) {
			arm.setRotation(parsed_input.rotation);
		}

		arm.setSpeed(parsed_input.x, parsed_input.y);
		tankTracks.move(parsed_input.a, parsed_input.b, 512);

		//update batteryPercentage
		//batteryPerc = (int)(((float)arm.getVoltage() - 99) / (126 - 99) * 100);
		//batteryPerc = batteryPerc > 100 ? 100 : batteryPerc < 0 ? 0 : batteryPerc;
		tempInt = getBatteryPercentage(arm);
		if (tempInt != 0) batteryPercBuffer += tempInt - batteryPerc;
		batteryPerc = batteryPercBuffer / batteryPercBufferSize;
		//std::cout << "batteryPerc=" << batteryPerc << std::endl;

		//if batteryPercentage to low shutdown pi
		//TODO: check on which batteryPercentage to shutdown the Pi
		if (parsed_input.doStop == true) std::cout << "doStop = true" << std::endl;
		if (batteryPerc < 10) std::cout << "batteryPerc < 10" << std::endl;
		if(batteryPerc < 10 || parsed_input.doStop == true){
			arm.stopMovement();
			arm.setServoValues({ 510,{ 200, 924, 689 }, 512, -1 }, 500);
			tankTracks.stopMotors();
			//talker.stopTalking();
			std::cout << "Application Stopped" << std::endl;
			break;
		}
		
		if (parsed_input.checkBattery) {
			const char* battery = std::to_string(batteryPerc).c_str();
			talker.sendMessage(battery);
		}

		if (parsed_input.gripper == 0) { arm.grab(true); }
		else if (parsed_input.gripper == 1) { arm.grab(false); }

		if (parsed_input.dance == 0) { arm.letsGetGroovy(); }
		else if (parsed_input.dance == 1) { std::cout << "Stop Dance has not been implemented yet" << std::endl; }

		if (parsed_input.lineDance == 0) { std::cout << "Start LineDance has not been implemented yet" << std::endl; }
		else if (parsed_input.lineDance == 1) { std::cout << "Stop LineDance has not been implemented yet" << std::endl; }


		free(tokenSwitch);

	}
	std::cout << "Listener Stopped" << std::endl;
    close(sockfd);
    return;
}
