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

//int socket(int domain, int type, int protocol);
int main(){
	int s;
	struct addrinfo hints, *res;

	getaddrinfo("www.google.com", "http", &hints, &res);

	s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	
	return 0;
}
