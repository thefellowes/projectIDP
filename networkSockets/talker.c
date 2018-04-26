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

#define SERVERPORT "1313"

int main(int argc, char *argv[]){
	int sokfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	
	//Throw an exeption if we dont get the required amount of parameters
	if(argc != 3){
		fprintf(stderr, "usage: talker hostname message\n");
		exit(1);
	}
	
	/*
	The hints argument points to an addrinfo structure that specifies
    criteria for selecting the socket address structures returned in the
    list pointed to by res.  If hints is not NULL it points to an
    addrinfo structure whose ai_family, ai_socktype, and ai_protocol
    specify criteria that limit the set of socket addresses returned by
    getaddrinfo()
	*/
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	
	if((rv = getaddrinfo(argv[1], SERVERPORT, &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
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
		return 2;
	}
	
	if((numbytes = sendto(sockfd, argv[2], strlen(argv[2]), 0, p->ai_addr, p->ai_addrlen)) == -1){
		perror("talker: sendto");
		exit(1);
	}
	
	freeaddrinfo(servinfo);
	
	printf("talkder: sent %d bytes to %s\n", numbytes, argv[1]);
	close(sockfd);
	
	return 0;
}
