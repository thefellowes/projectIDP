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
#include <assert.h>

#include <thread>
#include <chrono>

#include "dbg.h"
#include "parser.h"
#include "arm.h"
#include "AX12A.h"

#define MYPORT "1313"
#define MAXBUFLEN 100

#define DirectionPin (18u)
#define BaudRate (1000000ul)
#define IDturn (14u)
#define ID (3u)
#define ID1 (9u)
#define ID2 (61u)
#define Serial "/dev/ttyAMA0" 

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

int main(void){	
	AX12A ax12a;
	ax12a.begin(BaudRate, DirectionPin, Serial);

	std::vector<int> servos = {IDturn, ID, ID1, ID2};
	
	Arm arm(ax12a, servos);	
	arm.moveTo(-10, 0, 180, 512);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	arm.moveTo(-17.5, 7.5, 270, 512);
	
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	char buf[MAXBUFLEN];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP
	if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
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
		return 2;
	}
	freeaddrinfo(servinfo);
	printf("listener: waiting to recvfrom...\n");
	while (1) {
		addr_len = sizeof their_addr;
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) == -1) {
			perror("recvfrom");
			exit(1);
		}
		printf("listener: got packet from %s\n",
		inet_ntop(their_addr.ss_family,
		get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
		printf("listener: packet is %d bytes long\n", numbytes);
		buf[numbytes] = '\0';

		char** tokenSwitch;
		tokenSwitch = str_split(buf, '\n');

		//X, Y, A, B
		struct user_input parsed_input = parse_input(tokenSwitch);
		printf("Move : %f |  %f", parsed_input.x, parsed_input.y);
		arm.move(parsed_input.x, parsed_input.y);		

		free(tokenSwitch);
	}
	
	ax12a.end();
	close(sockfd);
	return 0;
}
