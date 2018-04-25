#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <cctype>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
int main(){
	struct addrinfo hints, *res;
	int sockfd;
	int yes=1;

	//Clear previous sessions
	if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1){
		perror("setsockopt");
		exit(1);
	}

	//Load the structs address
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; //grab the host ip (server)

	getaddrinfo(NULL, "3490", &hints, &res);

	//Make the socket
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	bind(sockfd, res->ai_addr, res->ai_addrlen);
	
	return 0;
}
