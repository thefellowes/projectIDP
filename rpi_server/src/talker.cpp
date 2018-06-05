#include "talker.h"
#include "AX12A.h"

Talker::Talker(AX12A &ax12a)
{
	this->ax12a = ax12a;
	stop = false;

	//create connection
	struct addrinfo hints;
	int rv;
	
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
}

void Talker::sendMessage(const char *message)
{
	int numbytes;

	for(int i = 0; i < 5; i++){
		if((numbytes = sendto(sockfd, message, strlen(message), 0, p->ai_addr, p->ai_addrlen)) == -1){
			perror("talker: sendto");
			exit(1);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	
	//printf("talker: sent %s to %s\n", message, IPAdress);
	//printf("talker: sent %d bytes to %s\n", numbytes, IPAdress);
}

//call this method in a thread
void Talker::startTalking()
{
	int batteryPerc;
	stop = false;
	
	while(!stop){
		batteryPerc = (int)(((float)ax12a.readVoltage(14u) - 99) / (126 - 99) * 100);//Broadcast id 254 not responding so using one of the servo id's
		batteryPerc = batteryPerc > 100 ? 100 : batteryPerc < 0 ? 0 : batteryPerc;
		const char* battery = std::to_string(batteryPerc).c_str();
		
		sendMessage(battery);

		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}

void Talker::stopTalking(){
	stop = true;
}

Talker::~Talker()
{
	//close connection
	freeaddrinfo(servinfo);

	close(sockfd);
}