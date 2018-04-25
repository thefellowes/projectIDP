#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <cctype>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(){
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo; //will point to the results
	
	char ip4[INET_ADDRSTRLEN]; // space to hold the IPv4 string
	struct sockaddr_in sa; // pretend this is loaded with something
	
	inet_ntop(AF_INET, &(sa.sin_addr), ip4, INET_ADDRSTRLEN);
	printf("The IPv4 address is: %s\n", ip4);
	
	memset(&hints, 0, sizeof hints); //make sure the struct is empty
	hints.ai_family = AF_UNSPEC; //Either ipv4 / 6
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	if((status = getaddrinfo("0.0.0.0", "3490", &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}
	
	//Do everything
	status = getaddrinfo("www.example.net", "3490", &hints, &servinfo);
	
	
	freeaddrinfo(servinfo);
	
	return 0;
}

