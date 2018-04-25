/*                   __
  ___ ___      __   /\_\    ___
/' __` __`\  /'__`\ \/\ \ /' _ `\
/\ \/\ \/\ \/\ \L\.\_\ \ \/\ \/\ \
\ \_\ \_\ \_\ \__/.\_\\ \_\ \_\ \_\
 \/_/\/_/\/_/\/__/\/_/ \/_/\/_/\/_/
*/


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "dbg.h"
#include "init.h"

#define PORT 1313
#define BACKLOG 10 // how many pending connections queue will hold
#define MAXDATASIZE 100 // max number of bytes we can get at once


/*
 * get sockaddr, either IPv4 or IPv6:
 */
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


/*
 * get the port from the cli-arguments or from the pre-defined standard port
 */
int get_port(int argc, char const *argv[])
{
	if (argc < 2)
		return PORT;

	return atoi(argv[1]);
}


int main(int argc, char const *argv[])
{
	u_short port = get_port(argc, argv);
	int server_sock = -1;
	int client_sock = -1;
	struct sockaddr_storage their_addr;
	socklen_t sin_size;
	char s[INET6_ADDRSTRLEN];

	server_sock = init(port);
	printf("server: listening on port %d...\n", port);

	while(1) {
		sin_size = sizeof their_addr;
		client_sock = accept(server_sock, (struct sockaddr *)&their_addr, &sin_size);

		if (client_sock == -1) {
			perror("accept");
			continue;
		}

		// print info about incoming request
		inet_ntop(their_addr.ss_family,	get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
		printf("server: got connection from %s\n", s);

		// handle_request(client_sock);
		// pool.enqueue(std::bind(handle_request, client_sock));
	}

	close(server_sock);
	return 1;
}
