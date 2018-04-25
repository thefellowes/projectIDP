#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(){
	//int connect(int sockfd, struct sockaddr *serv_addr, int addrlen);
	struct addrinfo hints, *res;
	int sockfd;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	getaddrinfo("www.google.com", "3490", &hints, &res);
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	
	connect(sockfd, res->ai_addr, rs->ai_addrlen);
	
	return 0;
}	
