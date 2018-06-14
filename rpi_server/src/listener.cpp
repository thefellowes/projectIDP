#include "listener.h"

Listener::Listener(const char* myPort) 
{
	IPStr = "-1";
	struct addrinfo hints, *servinfo;
	int rv;
	
	//char s[INET6_ADDRSTRLEN];
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP
	if ((rv = getaddrinfo(NULL, myPort, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(EXIT_FAILURE);
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next) {
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
}

//Start in its own thread
//void Listener::listen_t() {
//	int numbytes;
//	char buf[MAXBUFLEN];
//
//	printf("listener: waiting to recvfrom...\n");
//	while (1) 
//	{
//		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, p->ai_addr, p->ai_addrlen) == -1) {
//			perror("recvfrom");
//			exit(1);
//		}
//		//printf("listener: got packet from %s\n",
//		//inet_ntop(their_addr.ss_family,
//		//get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
//		//printf("listener: packet is %d bytes long\n", numbytes);
//		buf[numbytes] = '\0';
//
//		tokenSwitch = str_split(buf, '\n');
//
//		free(tokenSwitch);
//
//	}
//	std::cout << "Listener Stopped" << std::endl;
//    close(sockfd);
//    return;
//}

const char* Listener::getIP() {
	if (IPStr == "-1") {
		char s[INET6_ADDRSTRLEN];
		int numbytes;
		char buf[MAXBUFLEN];

		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN - 1, 0, (struct sockaddr *)&their_addr, &p->ai_addrlen)) == -1) {
			perror("recvfrom");
			exit(1);
		}

		return IPStr = inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
	}
	else {
		return IPStr;
	}
}

char** Listener::getToken() {
	int numbytes;
	char buf[MAXBUFLEN];

	if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1, 0, p->ai_addr, &p->ai_addrlen)) == -1) {
		perror("recvfrom");
		exit(1);
	}
	printf("listener: packet is %d bytes long\n", numbytes);
	buf[numbytes] = '\0';

	tokenSwitch = str_split(buf, '\n');

	return tokenSwitch;
}

user_input Listener::getParsedInput() {
	char** tokenSwitch;
	int numbytes;
	char buf[MAXBUFLEN];

	if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN - 1, 0, (struct sockaddr *)&their_addr, &p->ai_addrlen)) == -1) {
		perror("recvfrom");
		exit(1);
	}
	printf("listener: packet is %d bytes long\n", numbytes);
	buf[numbytes] = '\0';

	tokenSwitch = str_split(buf, '\n');

	user_input parsed_input = parse_input(tokenSwitch);

	free(tokenSwitch);

	return parsed_input;
}

// get sockaddr, IPv4 or IPv6:
void *Listener::get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

char** Listener::str_split(char* a_str, const char a_delim)
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

Listener::~Listener()
{
	//close connection
	//freeaddrinfo(servinfo);

	close(sockfd);
}

