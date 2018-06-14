#ifndef LISTENER_H
#define LISTENER_H

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <thread>
#include <functional>
#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <assert.h>

#include "parser.h"

#define MAXBUFLEN 100

class Listener{
	//private variables
	private:
		int sockfd;
		struct addrinfo *p;
		socklen_t* addr_len;
		char** tokenSwitch;

	//private functions
	private:
		void *get_in_addr(struct sockaddr *sa);
		char** str_split(char* a_str, const char a_delim);

	//public functions
	public:
		Listener(char* myPort);
		//void listen_t();
		char** getToken();
		user_input getParsedInput();
		~Listener();
};

#endif
