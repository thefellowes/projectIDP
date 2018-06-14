#include "talker.h"

Talker::Talker(const char* serverPort, const char* IPAddress)
{
	//create connection
	struct addrinfo hints;
	int rv;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	
	if((rv = getaddrinfo(IPAddress, serverPort, &hints, &servinfo)) != 0){
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
	
	//printf("talker: sent %s to %s\n", message, IPAddress);
	//printf("talker: sent %d bytes to %s\n", numbytes, IPAddress);
}

Talker::~Talker()
{
	//close connection
	freeaddrinfo(servinfo);

	close(sockfd);
}