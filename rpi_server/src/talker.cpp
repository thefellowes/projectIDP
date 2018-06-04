#include "talker.h"
#include "AX12A.h"

Talker::Talker(AX12A &ax12a)
{
	this->ax12a = ax12a;
	stop = false;
}

void Talker::stopTalking(){
	stop = true;
}

void Talker::startTalking(){
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;

	std::string batteryPerc;
	stop = false;
	
	//Throw an exeption if we dont get the required amount of parameters
	/**if(argc != 2){
		fprintf(stderr, "usage: talker hostname message\n");
		exit(1);
	}**/
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	
	if((rv = getaddrinfo(IPAdress, SERVERPORT, &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
	}
	
	//Go through the results
	for(p = servinfo; p != NULL; p = p->ai_next){
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			perror("talker: socket");
			continue;
		}
		break;
	}
	
	if(p == NULL){
		fprintf(stderr, "talker: faild to create socket \n");
	}

	while(!stop){
		batteryPerc = std::to_string((int)(((float)ax12a.readVoltage(254) - 9.9) / (11.1 - 9.9) * 100));//TODO: change 254 for a servoID if 254 not working
		const char* battery = batteryPerc.c_str();

		if((numbytes = sendto(sockfd, battery, strlen(battery), 0, p->ai_addr, p->ai_addrlen)) == -1){
			perror("talker: sendto");
			exit(1);
		}

		printf("talker: sent %d bytes to %s\n", numbytes, IPAdress);

		std::this_thread::sleep_for(std::chrono::seconds(10));
	}
	
	freeaddrinfo(servinfo);

	close(sockfd);
}
